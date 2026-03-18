#include "loader.h"
#include "libc.h"
#include "syscall.h"

#define MAX_SHARED_OBJECTS 16

static so_t loaded_sos[MAX_SHARED_OBJECTS];
static int so_count = 0;

/* ── ELF validation ── */
static int is_valid_elf(elf64_hdr_t *hdr) {
    if (!hdr) return 0;
    if (hdr->e_ident[EI_MAG0] != ELFMAG0) return 0;
    if (hdr->e_ident[EI_MAG1] != ELFMAG1) return 0;
    if (hdr->e_ident[EI_MAG2] != ELFMAG2) return 0;
    if (hdr->e_ident[EI_MAG3] != ELFMAG3) return 0;
    if (hdr->e_ident[EI_CLASS] != ELFCLASS64) return 0;
    if (hdr->e_ident[EI_DATA] != ELFDATA2LSB) return 0;
    if (hdr->e_machine != EM_X86_64) return 0;
    return 1;
}

/* ── load ELF file ── */
so_t *loader_load(const char *path) {
    if (so_count >= MAX_SHARED_OBJECTS) return NULL;
    
    int fd = sys_open(path, 0, 0);
    if (fd < 0) return NULL;
    
    /* read ELF header */
    uint8_t buf[4096];
    ssize_t nread = sys_read(fd, buf, sizeof(buf));
    if (nread < (ssize_t)sizeof(elf64_hdr_t)) {
        sys_close(fd);
        return NULL;
    }
    
    elf64_hdr_t *hdr = (elf64_hdr_t *)buf;
    if (!is_valid_elf(hdr)) {
        sys_close(fd);
        return NULL;
    }
    
    /* allocate shared object */
    so_t *so = &loaded_sos[so_count++];
    so->hdr = (elf64_hdr_t *)malloc(sizeof(elf64_hdr_t));
    memcpy(so->hdr, hdr, sizeof(elf64_hdr_t));
    
    /* calculate total size needed */
    uint64_t total_size = 0;
    elf64_phdr_t *phdr = (elf64_phdr_t *)(buf + hdr->e_phoff);
    for (int i = 0; i < hdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            uint64_t end = phdr[i].p_vaddr + phdr[i].p_memsz;
            if (end > total_size) total_size = end;
        }
    }
    
    /* allocate memory for shared object */
    so->base = (uint64_t)malloc(total_size);
    so->size = total_size;
    memset((void *)so->base, 0, total_size);
    
    /* load segments */
    for (int i = 0; i < hdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            uint64_t dst = so->base + phdr[i].p_vaddr;
            uint64_t src = (uint64_t)buf + phdr[i].p_offset;
            memcpy((void *)dst, (void *)src, phdr[i].p_filesz);
        }
    }
    
    /* find symbol table and string table */
    elf64_shdr_t *shdr = (elf64_shdr_t *)(buf + hdr->e_shoff);
    for (int i = 0; i < hdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_DYNSYM) {
            so->symtab = (elf64_sym_t *)(so->base + shdr[i].sh_addr);
            so->symcount = shdr[i].sh_size / sizeof(elf64_sym_t);
        }
        if (shdr[i].sh_type == SHT_STRTAB && i != hdr->e_shstrndx) {
            so->strtab = (char *)(so->base + shdr[i].sh_addr);
        }
    }
    
    sys_close(fd);
    return so;
}

/* ── resolve symbol ── */
void *loader_resolve(so_t *so, const char *symbol) {
    if (!so || !so->symtab || !so->strtab) return NULL;
    
    for (uint32_t i = 0; i < so->symcount; i++) {
        elf64_sym_t *sym = &so->symtab[i];
        const char *sym_name = so->strtab + sym->st_name;
        
        if (strcmp(sym_name, symbol) == 0) {
            return (void *)(so->base + sym->st_value);
        }
    }
    
    return NULL;
}

/* ── relocate ── */
int loader_relocate(so_t *so) {
    if (!so || !so->hdr) return -1;
    
    elf64_shdr_t *shdr = (elf64_shdr_t *)((uint8_t *)so->hdr + so->hdr->e_shoff);
    
    for (int i = 0; i < so->hdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_RELA) {
            elf64_rela_t *rela = (elf64_rela_t *)(so->base + shdr[i].sh_addr);
            uint32_t rela_count = shdr[i].sh_size / sizeof(elf64_rela_t);
            
            for (uint32_t j = 0; j < rela_count; j++) {
                uint64_t offset = so->base + rela[j].r_offset;
                uint32_t sym_idx = ELF64_R_SYM(rela[j].r_info);
                uint32_t rel_type = ELF64_R_TYPE(rela[j].r_info);
                
                uint64_t sym_value = 0;
                if (sym_idx > 0 && so->symtab) {
                    sym_value = so->base + so->symtab[sym_idx].st_value;
                }
                
                switch (rel_type) {
                case R_X86_64_64:
                    *(uint64_t *)offset = sym_value + rela[j].r_addend;
                    break;
                case R_X86_64_PC32:
                    *(uint32_t *)offset = (uint32_t)(sym_value + rela[j].r_addend - offset);
                    break;
                case R_X86_64_RELATIVE:
                    *(uint64_t *)offset = so->base + rela[j].r_addend;
                    break;
                case R_X86_64_GLOB_DAT:
                case R_X86_64_JUMP_SLOT:
                    *(uint64_t *)offset = sym_value;
                    break;
                default:
                    break;
                }
            }
        }
        
        if (shdr[i].sh_type == SHT_REL) {
            elf64_rel_t *rel = (elf64_rel_t *)(so->base + shdr[i].sh_addr);
            uint32_t rel_count = shdr[i].sh_size / sizeof(elf64_rel_t);
            
            for (uint32_t j = 0; j < rel_count; j++) {
                uint64_t offset = so->base + rel[j].r_offset;
                uint32_t sym_idx = ELF64_R_SYM(rel[j].r_info);
                uint32_t rel_type = ELF64_R_TYPE(rel[j].r_info);
                
                uint64_t sym_value = 0;
                if (sym_idx > 0 && so->symtab) {
                    sym_value = so->base + so->symtab[sym_idx].st_value;
                }
                
                switch (rel_type) {
                case R_X86_64_64:
                    *(uint64_t *)offset += sym_value;
                    break;
                case R_X86_64_PC32:
                    *(uint32_t *)offset += (uint32_t)(sym_value - offset);
                    break;
                case R_X86_64_RELATIVE:
                    *(uint64_t *)offset += so->base;
                    break;
                default:
                    break;
                }
            }
        }
    }
    
    return 0;
}

/* ── unload ── */
void loader_unload(so_t *so) {
    if (!so) return;
    if (so->hdr) free(so->hdr);
    if (so->base) free((void *)so->base);
    memset(so, 0, sizeof(so_t));
}

/* ── dlopen/dlsym/dlclose (POSIX-like API) ── */
typedef void *(*dlopen_func_t)(const char *filename, int flags);
typedef void *(*dlsym_func_t)(void *handle, const char *symbol);
typedef int (*dlclose_func_t)(void *handle);

void *dlopen(const char *filename, int flags) {
    (void)flags;
    return (void *)loader_load(filename);
}

void *dlsym(void *handle, const char *symbol) {
    so_t *so = (so_t *)handle;
    return loader_resolve(so, symbol);
}

int dlclose(void *handle) {
    so_t *so = (so_t *)handle;
    loader_unload(so);
    return 0;
}

const char *dlerror(void) {
    return "Dynamic loader error";
}

#ifndef _LOADER_H
#define _LOADER_H

#include <stdint.h>
#include <stddef.h>

/* ── ELF header ── */
#define EI_MAG0       0
#define EI_MAG1       1
#define EI_MAG2       2
#define EI_MAG3       3
#define EI_CLASS      4
#define EI_DATA       5
#define EI_VERSION    6
#define EI_OSABI      7
#define EI_ABIVERSION 8

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELFCLASS64 2
#define ELFDATA2LSB 1

#define ET_EXEC  2
#define ET_DYN   3
#define ET_REL   1

#define EM_X86_64 62

typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} elf64_hdr_t;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} elf64_phdr_t;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} elf64_shdr_t;

typedef struct {
    uint32_t st_name;
    uint8_t  st_info;
    uint8_t  st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
} elf64_sym_t;

typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
} elf64_rel_t;

typedef struct {
    uint64_t r_offset;
    uint64_t r_info;
    int64_t  r_addend;
} elf64_rela_t;

#define PT_LOAD 1
#define PT_DYNAMIC 3
#define PT_INTERP 3

#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_REL 9
#define SHT_RELA 4
#define SHT_DYNSYM 11

#define R_X86_64_64 1
#define R_X86_64_PC32 2
#define R_X86_64_GOT32 3
#define R_X86_64_PLT32 4
#define R_X86_64_COPY 5
#define R_X86_64_GLOB_DAT 6
#define R_X86_64_JUMP_SLOT 7
#define R_X86_64_RELATIVE 8

#define ELF64_R_SYM(i)   ((i) >> 32)
#define ELF64_R_TYPE(i)  ((i) & 0xffffffff)
#define ELF64_R_INFO(s,t) (((s) << 32) + (t))

/* ── shared object ── */
typedef struct {
    uint64_t base;
    uint64_t size;
    elf64_hdr_t *hdr;
    elf64_sym_t *symtab;
    char *strtab;
    uint32_t symcount;
} so_t;

/* ── loader API ── */
so_t *loader_load(const char *path);
void *loader_resolve(so_t *so, const char *symbol);
int loader_relocate(so_t *so);
void loader_unload(so_t *so);

#endif

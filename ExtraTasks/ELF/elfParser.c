#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>

#define ELF_FILE "firmware.elf"

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void check_elf_magic(Elf32_Ehdr *ehdr) {
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "Not a valid ELF file\n");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    FILE *f = fopen(ELF_FILE, "rb");
    if (!f) die("fopen");

    /* ---------------- ELF HEADER ---------------- */
    Elf32_Ehdr ehdr;
    fread(&ehdr, 1, sizeof(ehdr), f);
    check_elf_magic(&ehdr);

    printf("======= ELF HEADER =======\n");
    printf("Entry point : 0x%08X\n", ehdr.e_entry);
    printf("PH offset   : 0x%08X\n", ehdr.e_phoff);
    printf("SH offset   : 0x%08X\n", ehdr.e_shoff);
    printf("PH count    : %u\n", ehdr.e_phnum);
    printf("SH count    : %u\n", ehdr.e_shnum);

    /* ---------------- PROGRAM HEADERS ---------------- */
    printf("\n======= PROGRAM HEADERS (LOADABLE SEGMENTS) =======\n");

    Elf32_Phdr *phdrs = malloc(ehdr.e_phentsize * ehdr.e_phnum);
    if (!phdrs) die("malloc phdrs");

    fseek(f, ehdr.e_phoff, SEEK_SET);
    fread(phdrs, ehdr.e_phentsize, ehdr.e_phnum, f);

    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (phdrs[i].p_type == PT_LOAD) {
            printf("LOAD Segment %d\n", i);
            printf("  File offset : 0x%08X\n", phdrs[i].p_offset);
            printf("  Flash addr  : 0x%08X\n", phdrs[i].p_paddr);
            printf("  File size   : %u bytes\n", phdrs[i].p_filesz);
            printf("  Mem size    : %u bytes\n", phdrs[i].p_memsz);
            printf("  Flags       : 0x%X\n", phdrs[i].p_flags);
        }
    }

    /* ---------------- SECTION HEADERS ---------------- */
    printf("\n======= SECTION HEADERS =======\n");

    Elf32_Shdr *shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!shdrs) die("malloc shdrs");

    fseek(f, ehdr.e_shoff, SEEK_SET);
    fread(shdrs, ehdr.e_shentsize, ehdr.e_shnum, f);

    /* ---------------- SECTION NAME STRING TABLE ---------------- */
    Elf32_Shdr shstr = shdrs[ehdr.e_shstrndx];
    char *shstrtab = malloc(shstr.sh_size);
    if (!shstrtab) die("malloc shstrtab");

    fseek(f, shstr.sh_offset, SEEK_SET);
    fread(shstrtab, 1, shstr.sh_size, f);

    /* ---------------- WALK SECTIONS ---------------- */
    for (int i = 0; i < ehdr.e_shnum; i++) {
        const char *name = shstrtab + shdrs[i].sh_name;

        printf("Section %-16s Addr 0x%08X Size %6u\n",
               name,
               shdrs[i].sh_addr,
               shdrs[i].sh_size);
    }

    /* ---------------- READ RAW .text SECTION ---------------- */
    printf("\n======= READING .text SECTION =======\n");

    for (int i = 0; i < ehdr.e_shnum; i++) {
        const char *name = shstrtab + shdrs[i].sh_name;

        if (strcmp(name, ".text") == 0) {
            uint8_t *text = malloc(shdrs[i].sh_size);
            if (!text) die("malloc .text");

            fseek(f, shdrs[i].sh_offset, SEEK_SET);
            fread(text, 1, shdrs[i].sh_size, f);

            printf(".text loaded (%u bytes)\n", shdrs[i].sh_size);

            /* Example: print first 16 bytes */
            printf("First 16 bytes: ");
            for (int j = 0; j < 16 && j < shdrs[i].sh_size; j++)
                printf("%02X ", text[j]);
            printf("\n");

            free(text);
            break;
        }
    }

    /* ---------------- CLEANUP ---------------- */
    free(phdrs);
    free(shdrs);
    free(shstrtab);
    fclose(f);

    printf("\nELF parsing completed successfully.\n");
    return 0;
}
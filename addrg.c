#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <htslib/sam.h>

int main(int argc, char *argv[]) {
    bam_hdr_t *header;
    bam1_t *read;

    samFile * in, * of;

    if(argc < 3 || strcmp(argv[1], "-h") == 0) {
        printf("Usage: %s file.bam rg-name [sm-name=rg-name] [platform=ILLUMINA]\n", argv[0]);
        printf("       e.g. %s some.bam rg-id\n", argv[0]);
        printf("            %s some.bam rg-id sm-name\n", argv[0]);
        printf("adds a read-group to a bam writes to stdout\n");
        return -1;
    }

    char *rg = argv[2];
    char *sm = argv[3];
    if (argc >= 4)
        sm = rg;
    char *pl = "ILLUMINA";
    if (argc >= 5)
        pl = argv[4];

    in = strcmp(argv[1], "-")? sam_open(argv[1], "rb") : sam_open("/dev/stdin", "rb");

    header = sam_hdr_read(in);
    read = bam_init1();
    char *fmt = "@RG\tID:%s\tSM:%s\tPL:%s";

    of = sam_open("/dev/stdout", "wb");
    char *rg_full = malloc(sizeof(char) * (strlen(rg) + strlen(sm) + strlen(pl) + strlen(fmt)));
    int rg_len = sprintf(rg_full, fmt, rg, sm, pl);

    header->text = realloc(header->text, strlen(header->text) + rg_len);
    header->l_text = sprintf(header->text, "%s%s\n", header->text, rg_full);

    sam_hdr_write(of, header);

    while(sam_read1(in, header, read) > 1) {
        bam_aux_append(read, "RG", 'Z', strlen(rg) + 1, (uint8_t *)(rg));
        sam_write1(of, header, read);
    }

    sam_close(of);
    sam_close(in);
    free(rg_full);
    bam_destroy1(read);
    return 0;
}

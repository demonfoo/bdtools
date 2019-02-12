#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "bdtools/clpi_parse.h"
#include "bdtools/codecstrings.h"
#include "util.h"

static int verbose;

static void
_show_stream(CLPI_PROG_STREAM *ss, int level)
{
    str_t *lang;

    indent_printf(level, "Codec (%04x): %s", ss->coding_type,
                    _lookup_str(codec_map, ss->coding_type));
    indent_printf(level, "PID: %04x", ss->pid);
    switch (ss->coding_type) {
        case BD_STREAM_TYPE_VIDEO_MPEG1:
        case BD_STREAM_TYPE_VIDEO_MPEG2:
        case BD_STREAM_TYPE_VIDEO_VC1:
        case BD_STREAM_TYPE_VIDEO_H264:
        case BD_STREAM_TYPE_VIDEO_HEVC:
            indent_printf(level, "Format %02x: %s", ss->format,
                        _lookup_str(video_format_map, ss->format));
            indent_printf(level, "Rate %02x: %s", ss->rate,
                        _lookup_str(video_rate_map, ss->rate));
            indent_printf(level, "Aspect %02x: %s", ss->aspect,
                        _lookup_str(video_aspect_map, ss->aspect));
            indent_printf(level, "oc_flag %02x", ss->oc_flag);
            break;

        case BD_STREAM_TYPE_AUDIO_MPEG1:
        case BD_STREAM_TYPE_AUDIO_MPEG2:
        case BD_STREAM_TYPE_AUDIO_LPCM:
        case BD_STREAM_TYPE_AUDIO_AC3:
        case BD_STREAM_TYPE_AUDIO_DTS:
        case BD_STREAM_TYPE_AUDIO_TRUHD:
        case BD_STREAM_TYPE_AUDIO_AC3PLUS:
        case BD_STREAM_TYPE_AUDIO_DTSHD:
        case BD_STREAM_TYPE_AUDIO_DTSHD_MASTER:
            indent_printf(level, "Format %02x: %s", ss->format,
                        _lookup_str(audio_format_map, ss->format));
            indent_printf(level, "Rate %02x:", ss->rate,
                        _lookup_str(audio_rate_map, ss->rate));
            lang = str_substr((char*)ss->lang, 0, 3);
            indent_printf(level, "Language: %s", lang->buf);
            str_free(lang);
            free(lang);
            break;

        case BD_STREAM_TYPE_SUB_TEXT:
            indent_printf(level, "Char Code: %02x", ss->char_code);
        case BD_STREAM_TYPE_SUB_PG:
        case BD_STREAM_TYPE_SUB_IG:
        case 0xa0: // <- what is this
            lang = str_substr((char*)ss->lang, 0, 3);
            indent_printf(level, "Language: %s", lang->buf);
            str_free(lang);
            free(lang);
            break;

        default:
            fprintf(stderr, "unrecognized coding type %02x\n", ss->coding_type);
            break;
    };
}

static void
_show_clip_info(CLPI_CLIP_INFO *ci, int level)
{
    int ii;
    str_t *str;

    indent_printf(level, "Clip Info");
    indent_printf(level+1, "Clip Stream Type: %02x", ci->clip_stream_type);
    indent_printf(level+1, "Clip Application Type: %02x", 
                ci->application_type);
    indent_printf(level+1, "is_cc5: %s", ci->is_cc5 ? "True" : "False");
    indent_printf(level+1, "cc5 count: %d", ci->cc5_thingy_count);
    indent_printf(level+1, "TS Recording Rate: %u", ci->ts_recording_rate);
    indent_printf(level+1, "Number Source Packets: %u", ci->num_source_packets);
    // Show ts type info
    indent_printf(level+1, "TS Type Info");
    indent_printf(level+2, "Validity Flags %02x", ci->ts_type_info.validity);
    str = str_substr((char*)ci->ts_type_info.format_id, 0, 4);
    indent_printf(level+2, "Format Id %s", str->buf);
    str_free(str);
    free(str);
    // Show cc5 thing
    for (ii = 0; ii < ci->cc5_thingy_count; ii++) {
        indent_printf(level+1, "CC5 %d", ii);
        indent_printf(level+2, "Unknown %08x", ci->cc5_thingy[ii].unknown);
        str = str_substr((char*)ci->cc5_thingy[ii].file_id, 0, 5);
        indent_printf(level+2, "File Id %s", str->buf);
        str_free(str);
        free(str);
        str = str_substr((char*)&ci->cc5_thingy[ii].file_code, 0, 4);
        indent_printf(level+2, "File Code %s", str->buf);
        str_free(str);
        free(str);
    }
    printf("\n");
}

static void
_show_seq_info(CLPI_SEQ_INFO *si, int level)
{
    CLPI_ATC_SEQ *atc;
    CLPI_STC_SEQ *stc;
    int ii, jj;

    indent_printf(level, "Sequence Info");
    indent_printf(level+1, "Number ATC Sequences: %d", si->num_atc_seq);
    for (ii = 0; ii < si->num_atc_seq; ii++) {
        atc = &si->atc_seq[ii];
        indent_printf(level+1, "ATC Sequence %d", ii);
        indent_printf(level+2, "SPN ATC Start: %u", atc->spn_atc_start);
        indent_printf(level+2, "Offset STC Id: %d", atc->offset_stc_id);
        indent_printf(level+2, "Number STC Sequences: %d", atc->num_stc_seq);
        for (jj = 0; jj < atc->num_stc_seq; jj++) {
            stc = &atc->stc_seq[jj];
            indent_printf(level+2, "ATC Sequence %d", jj);
            indent_printf(level+3, "SPN STC Start: %u", stc->spn_stc_start);
            indent_printf(level+3, "PCR PID: %04x", stc->pcr_pid);
            indent_printf(level+3, "Presentation Start: %u", 
                            stc->presentation_start_time);
            indent_printf(level+3, "Presentation End: %u", 
                            stc->presentation_end_time);
        }
    }
}

static void
_show_prog_info(CLPI_PROG_INFO *pi, int level)
{
    CLPI_PROG *prog;
    int ii, jj;

    indent_printf(level, "Program Info");
    indent_printf(level+1, "Number Programs: %d", pi->num_prog);
    for (ii = 0; ii < pi->num_prog; ii++) {
        prog = &pi->progs[ii];
        indent_printf(level+1, "Program %d", ii);
        indent_printf(level+2, "SPN Program Sequence Start: %d", 
                        prog->spn_program_sequence_start);
        indent_printf(level+2, "Program Map PID: %d", prog->program_map_pid);
        indent_printf(level+2, "Number Streams: %d", prog->num_streams);
        indent_printf(level+2, "Number Groups: %d", prog->num_groups);
        for (jj = 0; jj < prog->num_streams; jj++) {
            indent_printf(level+2, "Stream %d", jj);
            _show_stream(&prog->streams[jj], level+3);
        }
    }
}

static void
_show_cpi_info(CLPI_CPI *cpi, int level)
{
    CLPI_EP_MAP_ENTRY *entry;
    CLPI_EP_COARSE *coarse;
    CLPI_EP_FINE *fine;
    int ii, jj, kk;

    indent_printf(level, "CPI");
    indent_printf(level+1, "Number Stream PID: %d", cpi->num_stream_pid);
    for (ii = 0; ii < cpi->num_stream_pid; ii++) {
        entry = &cpi->entry[ii];
        indent_printf(level+1, "Stream: %d", ii);
        indent_printf(level+2, "PID: %04x", entry->pid);
        indent_printf(level+2, "EP Stream Type: %d", entry->ep_stream_type);
        indent_printf(level+2, "Number EP Coarse: %d", entry->num_ep_coarse);
        indent_printf(level+2, "Number EP Fine: %d", entry->num_ep_fine);
        indent_printf(level+2, "EP Map Start: %d", 
                        entry->ep_map_stream_start_addr);
        for (jj = 0; jj < entry->num_ep_coarse; jj++) {
            coarse = &entry->coarse[jj];
            indent_printf(level+2, "Coarse: %d", jj);
            indent_printf(level+3, "Ref EP Fine: %d", coarse->ref_ep_fine_id);
            indent_printf(level+3, "PTS EP: %d", coarse->pts_ep);
            indent_printf(level+3, "SPN EP: %d", coarse->spn_ep);
        }
        for (jj = 0; jj < entry->num_ep_fine; jj++) {
            fine = &entry->fine[jj];
            indent_printf(level+2, "Fine: %d", jj);
            indent_printf(level+3, "Angle Change Point: %s", 
                fine->is_angle_change_point ? "True":"False");
            indent_printf(level+3, "I End Offset: %d", 
                fine->i_end_position_offset);
            indent_printf(level+3, "PTS EP: %d", fine->pts_ep);
            indent_printf(level+3, "SPN EP: %d", fine->spn_ep);
        }
        if (verbose) {
            uint64_t pts;
            uint32_t spn;

            indent_printf(level+2, "PTS - SPN Map");
            for (jj = 0; jj < entry->num_ep_coarse; jj++) {
                int start, end;

                indent_printf(level+3, "Coarse: %d", jj);
                coarse = &entry->coarse[jj];
                start = coarse->ref_ep_fine_id;
                if (jj < entry->num_ep_coarse - 1) {
                    end = entry->coarse[jj+1].ref_ep_fine_id;
                } else {
                    end = entry->num_ep_fine;
                }
                for (kk = start; kk < end; kk++) {
                    fine = &entry->fine[kk];
                    pts = ((uint64_t) (coarse->pts_ep & ~0x01) << 19) +
                            ((uint64_t)fine->pts_ep << 9);
                    spn = (coarse->spn_ep & ~0x1FFFF) + fine->spn_ep;
                    indent_printf(level+4, "PTS %8lu/%8lu -- SPN %u",
                        pts, pts >> 1, spn);
                }
            }
        }
    }
}


static void
_usage(char *cmd)
{
    fprintf(stderr, 
"Usage: %s -vcspi <clpi file> [<clpi file> ...]\n"
"With no options, produces no output (not very useful)\n"
"Options:\n"
"    v - Verbose output.\n"
"    c - Shows the Clip Info structure\n"
"    s - Shows the Sequence Info structure\n"
"    p - Shows the Program Info structure\n"
"    i - Shows the CPI. PTS to SPN map\n"
, cmd);

    exit(EXIT_FAILURE);
}

#define OPTS "vcspi"

int
main(int argc, char *argv[])
{
    CLPI_CL *cl;
    int opt;
    int opt_clip_info = 0, opt_seq_info = 0, opt_prog_info = 0;
    int opt_cpi_info = 0;
    int ii;

    do {
        opt = getopt(argc, argv, OPTS);
        switch (opt) {
            case -1: break;

            case 'v':
                verbose = 1;
                break;

            case 's':
                opt_seq_info = 1;
                break;

            case 'i':
                opt_cpi_info = 1;
                break;

            case 'c':
                opt_clip_info = 1;
                break;

            case 'p':
                opt_prog_info = 1;
                break;

            default:
                _usage(argv[0]);
                break;
        }
    } while (opt != -1);

    if (optind >= argc) {
        _usage(argv[0]);
    }

    for (ii = optind; ii < argc; ii++) {
        cl = clpi_parse(argv[ii], verbose);
        if (cl == NULL) {
            fprintf(stderr, "Parse failed\n");
            continue;
        }
        if (opt_clip_info) {
            // Show clip info
            _show_clip_info(&cl->clip, 1);
        }
        if (opt_seq_info) {
            // Show sequence info
            _show_seq_info(&cl->sequence, 1);
        }
        if (opt_prog_info) {
            // Show program info
            _show_prog_info(&cl->program, 1);
        }
        if (opt_cpi_info) {
            // Show cpi
            _show_cpi_info(&cl->cpi, 1);
        }
        clpi_free(&cl);
    }
    return 0;
}


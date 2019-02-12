#ifndef _CODECSTRINGS_H_
#define _CODECSTRINGS_H_

#include "bdtools/bdparse.h"

typedef struct {
    int value;
    char *str;
} value_map_t;

static inline char*
_lookup_str(value_map_t *map, int val)
{
    int ii;

    for (ii = 0; map[ii].str; ii++) {
        if (val == map[ii].value) {
            return map[ii].str;
        }
    }
    return "?";
}

value_map_t codec_map[] = {
    {BD_STREAM_TYPE_VIDEO_MPEG1, "MPEG-1 Video"},
    {BD_STREAM_TYPE_VIDEO_MPEG2, "MPEG-2 Video"},
    {BD_STREAM_TYPE_AUDIO_MPEG1, "MPEG-1 Audio"},
    {BD_STREAM_TYPE_AUDIO_MPEG2, "MPEG-2 Audio"},
    {BD_STREAM_TYPE_AUDIO_LPCM,  "LPCM"},
    {BD_STREAM_TYPE_AUDIO_AC3,   "AC-3"},
    {BD_STREAM_TYPE_AUDIO_DTS,   "DTS"},
    {BD_STREAM_TYPE_AUDIO_TRUHD, "TrueHD"},
    {BD_STREAM_TYPE_AUDIO_AC3PLUS, "E-AC-3"},
    {BD_STREAM_TYPE_AUDIO_DTSHD, "DTS-HD High Resolution"},
    {BD_STREAM_TYPE_AUDIO_DTSHD_MASTER, "DTS-HD Master Audio"},
    {BD_STREAM_TYPE_VIDEO_VC1,   "VC-1"},
    {BD_STREAM_TYPE_VIDEO_H264,  "H.264/MPEG-4 AVC"},
    {BD_STREAM_TYPE_VIDEO_HEVC,  "HEVC/MPEG-H Part 2"},
    {BD_STREAM_TYPE_SUB_PG,      "Presentation Graphics"},
    {BD_STREAM_TYPE_SUB_IG,      "Interactive Graphics"},
    {BD_STREAM_TYPE_SUB_TEXT,    "Text Subtitle"},
    {0,                          NULL}
};

value_map_t video_format_map[] = {
    {0, "Reserved"},
    {BD_VIDEO_FORMAT_480I,  "480i"},
    {BD_VIDEO_FORMAT_576I,  "576i"},
    {BD_VIDEO_FORMAT_480P,  "480p"},
    {BD_VIDEO_FORMAT_1080I, "1080i"},
    {BD_VIDEO_FORMAT_720P,  "720p"},
    {BD_VIDEO_FORMAT_1080P, "1080p"},
    {BD_VIDEO_FORMAT_576P,  "576p"},
    {BD_VIDEO_FORMAT_2160P, "2160p"},
    {0, NULL}
};

value_map_t video_rate_map[] = {
    {0,                        "Reserved1"},
    {BD_VIDEO_RATE_24000_1001, "23.976"},
    {BD_VIDEO_RATE_24,         "24"},
    {BD_VIDEO_RATE_25,         "25"},
    {BD_VIDEO_RATE_30000_1001, "29.97"},
    {5,                        "Reserved2"},
    {BD_VIDEO_RATE_50,         "50"},
    {BD_VIDEO_RATE_60000_1001, "59.94"},
    {0,                        NULL}
};

value_map_t video_aspect_map[] = {
    {0,                    "Reserved1"},
    {1,                    "Reserved2"},
    {BD_ASPECT_RATIO_4_3,  "4:3"},
    {BD_ASPECT_RATIO_16_9, "16:9"},
    {0,                    NULL}
};

value_map_t audio_format_map[] = {
    {0, "Reserved1"},
    {BD_AUDIO_FORMAT_MONO,       "Mono"},
    {2,                          "Reserved2"},
    {BD_AUDIO_FORMAT_STEREO,     "Stereo"},
    {4,                          "Reserved3"},
    {5,                          "Reserved4"},
    {BD_AUDIO_FORMAT_MULTI_CHAN, "Multi Channel"},
    {BD_AUDIO_FORMAT_COMBO,      "Combo"},
    {0,                          NULL}
};

value_map_t audio_rate_map[] = {
    {0,                       "Reserved1"},
    {BD_AUDIO_RATE_48,        "48 Khz"},
    {2,                       "Reserved2"},
    {3,                       "Reserved3"},
    {BD_AUDIO_RATE_96,        "96 Khz"},
    {BD_AUDIO_RATE_192,       "192 Khz"},
    {BD_AUDIO_RATE_192_COMBO, "48/192 Khz"},
    {BD_AUDIO_RATE_96_COMBO,  "48/96 Khz"},
    {0,                       NULL}
};

#endif

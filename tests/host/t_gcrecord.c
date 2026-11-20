/*******************************************************************
 * File:        t_gcrecord
 * Purpose:     Tests of the recording GContext (h/gcrecord): every
 *              gcontext_t operation is driven through the function
 *              table and its record is checked.
 * Author:      Gerph
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gcontext.h"
#include "gcrecord.h"
#include "t_check.h"

#define RED   COLOUR_RGB(255, 0, 0)
#define BLUE  COLOUR_RGB(0, 0, 255)

static void test_shapes(void)
{
    gcontext_t *gc = gcontext_initrecord();
    const gcrecord_t *r;

    CHECK(gc != NULL, (""));
    if (gc == NULL)
    {
        return;
    }
    CHECK(gcrecord_count(gc) == 0 && gcrecord_get(gc, 0) == NULL, ("empty"));

    gc->rectangle_fill(gc, RED, 1, 2, 3, 4);
    gc->rectangle_outline(gc, BLUE, -5, -6, 7, 8);
    gc->triangle_fill(gc, RED, 0, 0, 10, 0, 5, 9);
    gc->line_start(gc, BLUE);
    gc->line_line(gc, 1, 2, 3, 4);
    gc->line_dotted(gc, 5, 6, 7, 8);
    gc->line_dashed(gc, 9, 10, 11, 12);
    gc->line_end(gc);
    gc->fill_start(gc, RED);
    gc->fill_move(gc, 20, 21);
    gc->fill_line(gc, 22, 23);
    gc->fill_close(gc);
    gc->fill_end(gc);

    CHECK(gcrecord_count(gc) == 13, ("count %d", gcrecord_count(gc)));
    r = gcrecord_get(gc, 0);
    CHECK(r->kind == GCR_RECTANGLE_FILL && r->colour == RED && r->x0 == 1
          && r->y0 == 2 && r->x1 == 3 && r->y1 == 4, ("rectangle_fill"));
    r = gcrecord_get(gc, 1);
    CHECK(r->kind == GCR_RECTANGLE_OUTLINE && r->colour == BLUE && r->x0 == -5
          && r->y0 == -6 && r->x1 == 7 && r->y1 == 8, ("rectangle_outline"));
    r = gcrecord_get(gc, 2);
    CHECK(r->kind == GCR_TRIANGLE_FILL && r->x1 == 10 && r->x2 == 5 && r->y2 == 9,
          ("triangle_fill"));
    CHECK(gcrecord_get(gc, 3)->kind == GCR_LINE_START
          && gcrecord_get(gc, 3)->colour == BLUE, ("line_start"));
    CHECK(gcrecord_get(gc, 4)->kind == GCR_LINE_LINE && gcrecord_get(gc, 4)->x1 == 3,
          ("line_line"));
    CHECK(gcrecord_get(gc, 5)->kind == GCR_LINE_DOTTED && gcrecord_get(gc, 5)->y0 == 6,
          ("line_dotted"));
    CHECK(gcrecord_get(gc, 6)->kind == GCR_LINE_DASHED && gcrecord_get(gc, 6)->y1 == 12,
          ("line_dashed"));
    CHECK(gcrecord_get(gc, 7)->kind == GCR_LINE_END, ("line_end"));
    CHECK(gcrecord_get(gc, 8)->kind == GCR_FILL_START
          && gcrecord_get(gc, 8)->colour == RED, ("fill_start"));
    CHECK(gcrecord_get(gc, 9)->kind == GCR_FILL_MOVE && gcrecord_get(gc, 9)->x0 == 20
          && gcrecord_get(gc, 9)->y0 == 21, ("fill_move"));
    CHECK(gcrecord_get(gc, 10)->kind == GCR_FILL_LINE && gcrecord_get(gc, 10)->x0 == 22,
          ("fill_line"));
    CHECK(gcrecord_get(gc, 11)->kind == GCR_FILL_CLOSE
          && gcrecord_get(gc, 12)->kind == GCR_FILL_END, ("fill_close, fill_end"));

    CHECK(gcrecord_count_kind(gc, GCR_LINE_LINE) == 1
          && gcrecord_count_kind(gc, GCR_RECTANGLE_FILL) == 1
          && gcrecord_count_kind(gc, GCR_TEXT_PAINT) == 0, ("count by kind"));
    CHECK(gcrecord_get(gc, -1) == NULL && gcrecord_get(gc, 13) == NULL,
          ("out of range"));

    gcrecord_clear(gc);
    CHECK(gcrecord_count(gc) == 0, ("clear"));
    gcrecord_destroy(gc);
}

static void test_text(void)
{
    gcontext_t *gc = gcontext_initrecord();
    font_t font;
    stringbounds_t b;
    bounds_t em;
    coords_t end;
    const gcrecord_t *r;

    font = gc->text_findfont(gc, "Homerton.Medium", 320, 320);
    CHECK(font != FONT_NONE, ("findfont"));
    r = gcrecord_get(gc, 0);
    CHECK(r->kind == GCR_TEXT_FINDFONT && strcmp(r->text, "Homerton.Medium") == 0
          && r->x0 == 320 && r->y0 == 320 && r->font == font, ("findfont record"));
    CHECK(gc->text_readhandle(gc, font) != 0, ("readhandle"));

    /* The default metrics: 8 wide, ascent 12, descent 4 */
    gc->text_getstringsize(gc, font, &b, -1, "hello", GCONTEXT_ZEROTERM,
                           GCONTEXT_NOSPLIT);
    CHECK(b.xoffset == 40 && b.ascent == 12 && b.descent == 4 && b.charoffset == 5,
          ("string size %ld", (long)b.xoffset));
    gc->text_getstringsize(gc, font, &b, -1, "hello", 3, GCONTEXT_NOSPLIT);
    CHECK(b.xoffset == 24, ("an explicit length"));
    em = gc->text_getemsize(gc, font);
    CHECK(em.width == 8 && em.height == 16, ("em size"));

    gcrecord_set_font_metrics(gc, 10, 20, 5);
    gc->text_getstringsize(gc, font, &b, -1, "abc", GCONTEXT_ZEROTERM, GCONTEXT_NOSPLIT);
    CHECK(b.xoffset == 30 && b.ascent == 20 && b.descent == 5, ("set metrics"));

    /* A limit: a split character gives the break; none gives charoffset 0 */
    gc->text_getstringsize(gc, font, &b, 55, "ab cd ef", GCONTEXT_ZEROTERM, ' ');
    CHECK(b.charoffset == 5 && b.xoffset == 50, ("split at the last space that fits: %ld %ld",
                                               (long)b.charoffset, (long)b.xoffset));
    gc->text_getstringsize(gc, font, &b, 25, "abcdef", GCONTEXT_ZEROTERM, ' ');
    CHECK(b.charoffset == 0, ("no split character: charoffset 0"));

    end = gc->text_paint(gc, font, 100, 200, (colour_t)COLOUR_NONE, RED, "Hello", GCONTEXT_ZEROTERM);
    CHECK(end.x == 150 && end.y == 200, ("paint returns the end: %ld", (long)end.x));
    r = gcrecord_get(gc, gcrecord_count(gc) - 1);
    CHECK(r->kind == GCR_TEXT_PAINT && r->x0 == 100 && r->y0 == 200
          && r->colour == RED && r->colour2 == COLOUR_NONE
          && strcmp(r->text, "Hello") == 0 && r->length == GCONTEXT_ZEROTERM
          && r->font == font, ("paint record"));

    /* An explicit length cuts the text; a long text is kept to 79 characters */
    gc->text_paint(gc, font, 0, 0, 0, 0, "Hello", 2);
    CHECK(strcmp(gcrecord_get(gc, gcrecord_count(gc) - 1)->text, "He") == 0, ("cut"));
    {
        char longtext[200];

        memset(longtext, 'x', sizeof(longtext) - 1);
        longtext[sizeof(longtext) - 1] = '\0';
        gc->text_paint(gc, font, 0, 0, 0, 0, longtext, GCONTEXT_ZEROTERM);
        CHECK(strlen(gcrecord_get(gc, gcrecord_count(gc) - 1)->text) == GCR_TEXT_MAX,
              ("a long text is truncated"));
    }

    end = gc->text_paintattrib(gc, font, 10, 20, 110, 60, "Hi", GCONTEXT_ZEROTERM, BLUE,
                               RED, FONTATTRIB_ALIGN_HCENTRE | FONTATTRIB_UNDERLINE);
    r = gcrecord_get(gc, gcrecord_count(gc) - 1);
    CHECK(r->kind == GCR_TEXT_PAINTATTRIB && r->x0 == 10 && r->y1 == 60
          && r->attrib == (FONTATTRIB_ALIGN_HCENTRE | FONTATTRIB_UNDERLINE)
          && r->colour == RED && r->colour2 == BLUE && end.x == 30, ("paintattrib"));

    gc->text_losefont(gc, font);
    r = gcrecord_get(gc, gcrecord_count(gc) - 1);
    CHECK(r->kind == GCR_TEXT_LOSEFONT && r->font == font, ("losefont"));
    gcrecord_destroy(gc);
}

static void test_clip(void)
{
    gcontext_t *gc = gcontext_initrecord();
    bbox_t box;
    const gcrecord_t *r;

    gc->rectangle_fill(gc, RED, 0, 0, 1, 1);
    CHECK(!gcrecord_get(gc, 0)->has_clip, ("no clip at first"));

    box.x0 = 10; box.y0 = 20; box.x1 = 30; box.y1 = 40;
    CHECK(gc->clip_rectangle(gc, &box), ("clip_rectangle"));
    CHECK(gc->clipstate == cs_clip && gc->clipping.x0 == 10 && gc->clipping.y1 == 40,
          ("the context's clip state"));
    gc->rectangle_fill(gc, RED, 0, 0, 1, 1);
    r = gcrecord_get(gc, 2);
    CHECK(r->has_clip && r->clip.x0 == 10 && r->clip.y0 == 20 && r->clip.x1 == 30
          && r->clip.y1 == 40, ("a drawing record keeps the clip it was made under"));
    r = gcrecord_get(gc, 1);
    CHECK(r->kind == GCR_CLIP && r->has_clip && r->x0 == 10 && r->y1 == 40, ("the clip record"));

    gc->clip_rectangle(gc, NULL);
    gc->rectangle_fill(gc, RED, 0, 0, 1, 1);
    CHECK(gc->clipstate == cs_noclip && !gcrecord_get(gc, 3)->has_clip
          && !gcrecord_get(gc, 4)->has_clip, ("clip cleared"));
    CHECK(gcrecord_get(gc, 3)->kind == GCR_CLIP && !gcrecord_get(gc, 3)->has_clip,
          ("the clear is recorded"));
    gcrecord_destroy(gc);
}

static void test_dump_and_growth(void)
{
    gcontext_t *gc = gcontext_initrecord();
    char text[1024];
    char small[16];
    size_t needed;
    int i;

    gc->rectangle_fill(gc, RED, 0, 0, 100, 50);
    gc->line_start(gc, BLUE);
    gc->line_line(gc, 0, 0, 100, 50);
    gc->line_end(gc);
    needed = gcrecord_dump(gc, text, sizeof(text));
    CHECK(needed == strlen(text), ("the dump length %lu and %lu", (unsigned long)needed,
                                   (unsigned long)strlen(text)));
    CHECK(strcmp(text, "rectangle_fill colour=0000ff00 0,0 100,50\n"
                       "line_start colour=ff000000\n"
                       "line_line 0,0 100,50\n"
                       "line_end\n") == 0, ("the dump text:\n%s", text));

    /* A buffer that is too small gets a terminated, cut dump, and the full size is
     * reported */
    CHECK(gcrecord_dump(gc, small, sizeof(small)) == needed && strlen(small) < 16,
          ("a short buffer"));
    CHECK(gcrecord_dump(gc, NULL, 0) == needed, ("sizing with no buffer"));

    /* Many records: the store grows */
    for (i = 0; i < 5000; i++)
    {
        gc->rectangle_fill(gc, RED, i, i, i + 1, i + 1);
    }
    CHECK(gcrecord_count(gc) == 5004 && gcrecord_get(gc, 5003)->x0 == 4999,
          ("5000 more records"));
    gcrecord_destroy(gc);
    gcrecord_destroy(NULL);
}

/* The image operations: recorded, with what they were given, and they report success */
static void test_images(void)
{
    gcontext_t *gc = gcontext_initrecord();
    const gcrecord_t *r;
    static const char data[4] = { 1, 2, 3, 4 };
    int dummy_area;
    char dump[512];

    CHECK(gc->image_plot_file != NULL && gc->image_plot_data != NULL && gc->tile_fill != NULL,
          ("the image operations are provided"));
    CHECK(gc->image_plot_file(gc, 0xAFF, "Pictures.Logo", 10, 20, 110, 70), ("plot a file"));
    CHECK(gc->image_plot_data(gc, GCONTEXT_FILETYPE_SPRITE, data, sizeof(data), 0, 0, 8, 8),
          ("plot data"));
    CHECK(gc->tile_fill(gc, GCONTEXT_FILETYPE_SPRITE, &dummy_area, "tile", 0, 0, 640, 480),
          ("tile by name"));
    CHECK(gc->tile_fill(gc, GCONTEXT_FILETYPE_SPRITE, GCONTEXT_NO_SPRITE_AREA, data, 1, 2, 3, 4),
          ("tile by pointer"));
    CHECK(gcrecord_count(gc) == 4, ("%d records", gcrecord_count(gc)));

    r = gcrecord_get(gc, 0);
    CHECK(r->kind == GCR_IMAGE_PLOT_FILE && r->attrib == 0xAFF && strcmp(r->text, "Pictures.Logo") == 0
          && r->x0 == 10 && r->y0 == 20 && r->x1 == 110 && r->y1 == 70, ("the file record"));
    r = gcrecord_get(gc, 1);
    CHECK(r->kind == GCR_IMAGE_PLOT_DATA && r->attrib == 0xFF9 && r->length == 4
          && r->x1 == 8 && r->y1 == 8, ("the data record keeps its length"));
    r = gcrecord_get(gc, 2);
    CHECK(r->kind == GCR_TILE_FILL && r->attrib == 0xFF9 && r->length == 0
          && strcmp(r->text, "tile") == 0 && r->x1 == 640 && r->y1 == 480, ("the named tile record"));
    r = gcrecord_get(gc, 3);
    CHECK(r->kind == GCR_TILE_FILL && r->length == 1 && r->text[0] == '\0' && r->x0 == 1,
          ("the tile by pointer is marked, with no name"));

    gcrecord_dump(gc, dump, sizeof(dump));
    CHECK(strcmp(dump, "image_plot_file filetype=aff \"Pictures.Logo\" 10,20 110,70\n"
                       "image_plot_data filetype=ff9 length=4 0,0 8,8\n"
                       "tile_fill filetype=ff9 \"tile\" 0,0 640,480\n"
                       "tile_fill filetype=ff9 pointer 1,2 3,4\n") == 0, ("the dump:\n%s", dump));
    gcrecord_destroy(gc);
}

int main(void)
{
    test_shapes();
    test_text();
    test_clip();
    test_dump_and_growth();
    test_images();
    return t_check_finish("t_gcrecord");
}

#include <stdlib.h>
#include <string.h>
#include <vips/vips.h>
#include <vips/foreign.h>
#include <vips/vector.h>

// Version compatibility macros
#if (VIPS_MAJOR_VERSION == 7 && VIPS_MINOR_VERSION < 41)
#define VIPS_ANGLE_D0 VIPS_ANGLE_0
#define VIPS_ANGLE_D90 VIPS_ANGLE_90
#define VIPS_ANGLE_D180 VIPS_ANGLE_180
#define VIPS_ANGLE_D270 VIPS_ANGLE_270
#endif

#define EXIF_IFD0_ORIENTATION "exif-ifd0-Orientation"
#define INT_TO_GBOOLEAN(bool) (bool > 0 ? TRUE : FALSE)

// Image type enums with packed values
enum types {
    UNKNOWN = 0,
    JPEG,
    WEBP,
    PNG,
    TIFF,
    GIF,
    PDF,
    SVG,
    MAGICK,
    HEIF,
    AVIF,
    JXL
};

// Optimized structs with aligned fields
typedef struct {
    const char *Text;
    const char *Font;
} WatermarkTextOptions;

typedef struct {
    int Width;
    int DPI;
    int Margin;
    int NoReplicate;
    float Opacity;
    double Background[3];
} WatermarkOptions;

typedef struct {
    int Left;
    int Top;
    float Opacity;
} WatermarkImageOptions;

// Core image operations
static inline int has_alpha_channel(VipsImage *image) {
    return (image->Bands == 2 && image->Type == VIPS_INTERPRETATION_B_W) ||
           (image->Bands == 4 && image->Type != VIPS_INTERPRETATION_CMYK) ||
           (image->Bands == 5 && image->Type == VIPS_INTERPRETATION_CMYK);
}

static inline unsigned long has_profile_embed(VipsImage *image) {
    return vips_image_get_typeof(image, VIPS_META_ICC_NAME);
}

static inline void remove_profile(VipsImage *image) {
    vips_image_remove(image, VIPS_META_ICC_NAME);
}

// Image loading operations
int vips_init_image(void *buf, size_t len, int imageType, VipsImage **out);
int vips_jpegload_buffer_shrink(void *buf, size_t len, VipsImage **out, int shrink);
int vips_webpload_buffer_shrink(void *buf, size_t len, VipsImage **out, int shrink);

// Image saving operations
int vips_jpegsave_bridge(VipsImage *in, void **buf, size_t *len, int strip, int quality, int interlace);
int vips_pngsave_bridge(VipsImage *in, void **buf, size_t *len, int strip, int compression, int quality, int interlace, int palette, int speed);
int vips_webpsave_bridge(VipsImage *in, void **buf, size_t *len, int strip, int quality, int lossless);
int vips_tiffsave_bridge(VipsImage *in, void **buf, size_t *len);
int vips_heifsave_bridge(VipsImage *in, void **buf, size_t *len, int strip, int quality, int lossless);
int vips_avifsave_bridge(VipsImage *in, void **buf, size_t *len, int strip, int quality, int lossless, int speed);
int vips_gifsave_bridge(VipsImage *in, void **buf, size_t *len, int strip);
int vips_jxlsave_bridge(VipsImage *in, void **buf, size_t *len, int strip, int quality, int lossless);

// Image transformation operations
int vips_autorot_bridge(VipsImage *in, VipsImage **out);
int vips_resize_bridge(VipsImage *in, VipsImage **out, double scale);
int vips_affine_interpolator(VipsImage *in, VipsImage **out, double a, double b, double c, double d, VipsInterpolate *interpolator, int extend);
int vips_rotate_bridge(VipsImage *in, VipsImage **out, int angle);
int vips_flip_bridge(VipsImage *in, VipsImage **out, int direction);
int vips_zoom_bridge(VipsImage *in, VipsImage **out, int xfac, int yfac);
int vips_embed_bridge(VipsImage *in, VipsImage **out, int left, int top, int width, int height, int extend, double r, double g, double b);
int vips_extract_area_bridge(VipsImage *in, VipsImage **out, int left, int top, int width, int height);
int vips_smartcrop_bridge(VipsImage *in, VipsImage **out, int width, int height);
int vips_find_trim_bridge(VipsImage *in, int *top, int *left, int *width, int *height, double r, double g, double b, double threshold);

// Color operations
int vips_icc_transform_bridge(VipsImage *in, VipsImage **out, const char *output_icc_profile);
int vips_icc_transform_with_default_bridge(VipsImage *in, VipsImage **out, const char *output_icc_profile, const char *input_icc_profile);
int vips_colourspace_bridge(VipsImage *in, VipsImage **out, VipsInterpretation space);
int vips_colourspace_issupported_bridge(VipsImage *in);
VipsInterpretation vips_image_guess_interpretation_bridge(VipsImage *in);

// Effect operations
int vips_gaussblur_bridge(VipsImage *in, VipsImage **out, double sigma, double min_ampl);
int vips_sharpen_bridge(VipsImage *in, VipsImage **out, int radius, double x1, double y2, double y3, double m1, double m2);
int vips_gamma_bridge(VipsImage *in, VipsImage **out, double exponent);
int vips_brightness_bridge(VipsImage *in, VipsImage **out, double k);
int vips_contrast_bridge(VipsImage *in, VipsImage **out, double k);

// Watermark operations
int vips_watermark(VipsImage *in, VipsImage **out, WatermarkTextOptions *to, WatermarkOptions *o);
int vips_watermark_image(VipsImage *in, VipsImage *sub, VipsImage **out, WatermarkImageOptions *o);

// Type checking operations
int vips_type_find_bridge(int t);
int vips_type_find_save_bridge(int t);
int vips_is_16bit(VipsInterpretation interpretation);

// EXIF operations
const char *vips_exif_tag(VipsImage *image, const char *tag);
int vips_exif_tag_to_int(VipsImage *image, const char *tag);
int vips_exif_orientation(VipsImage *image);

// Cache and memory operations
void vips_enable_cache_set_trace();

// Utility functions
const char *vips_enum_nick_bridge(VipsImage *image);
int interpolator_window_size(char const *name);
int vips_shrink_bridge(VipsImage *in, VipsImage **out, double xshrink, double yshrink);
int vips_reduce_bridge(VipsImage *in, VipsImage **out, double xshrink, double yshrink, int kernel);
int vips_flatten_background_brigde(VipsImage *in, VipsImage **out, double r, double g, double b);
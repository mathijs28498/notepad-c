typedef struct gfx_s gfx_t;
typedef struct gfx_vtable_s gfx_vtable_t;

struct gfx_s
{
    const gfx_vtable_t *vtable;
    void *ctx;
};

struct gfx_vtable_s
{
    int (*update_text)(const void *ctx);
};
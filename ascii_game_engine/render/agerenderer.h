/*
** This source file is part of AGE
**
** For the latest info, see http://code.google.com/p/ascii-game-engine/
**
** Copyright (c) 2011 Tony & Tony's Toy Game Development Team
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of
** this software and associated documentation files (the "Software"), to deal in
** the Software without restriction, including without limitation the rights to
** use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
** the Software, and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
** IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __AGE_RENDERER_H__
#define __AGE_RENDERER_H__

#include "../ageconfig.h"
#include "../common/agetype.h"
#include "../common/ageallocator.h"
#include "../common/agelist.h"
#include "../common/agehashtable.h"
#include "../common/ageparamset.h"
#include "../message/agemessage.h"
#include "../controller/agecontroller.h"

#define MAX_CACHED_FRAME_COUNT 16

/**
 * @brief visible visibility
 */
static const s32 VISIBILITY_VISIBLE = 2;
/**
 * @brief disappearing visibility
 */
static const s32 VISIBILITY_DISAPPEARING = 1;
/**
 * @brief hiden visibility
 */
static const s32 VISIBILITY_HIDEN = 0;

/**
 * @brief named frame prefix, for data string parsing
 */
static const s8 NAMED_FRAME_PREFIX = '@';

/**
 * @brief default z-order
 */
static const s32 DEFAULT_Z_ORDER = 0x0FFFFFFF;

/**
 * @brief color used for erase a pixel
 */
static const Color ERASE_PIXEL_COLOR = -1;

/**
 * @brief shape used for erase a pixel
 */
static const s8 ERASE_PIXEL_SHAPE = ' ';

/**
 * @brief invalid frame index
 */
static const s32 INVALID_FRAME_INDEX = -1;

/**
 * @brief empty physics mode
 */
static const u32 PHYSICS_MODE_NULL = 0;

/**
 * @brief obstacle physics mode
 */
static const u32 PHYSICS_MODE_OBSTACLE = (1 << 0);

/**
 * @brief checker physics mode
 */
static const u32 PHYSICS_MODE_CHECKER = (1 << 1);

/**
 * @brief font structure
 */
typedef struct Font {
	Color color; /**< font color */
} Font;

struct Frame;
struct Sprite;
struct Canvas;

/**
 * @brief pixel structure
 */
typedef struct Pixel {
	/* common */
	struct Frame* parent; /**< parent frame object */
	s8 shape;             /**< shape data */
	Color color;          /**< color value */
	s32 zorder;           /**< z-order of this pixel */
	union {
		/* for sprite frame */
		struct {
			s8 brush;     /**< brush data, used with palete to paint a pixel */
		};
		/* for canvas frame buffer */
		struct {
			struct Frame* owner_frames[MAX_CACHED_FRAME_COUNT]; /**< owner frames */
			s32 frame_count;                                    /**< owner frames count */
		};
	};
} Pixel;

/**
 * @brief frame structure
 */
typedef struct Frame {
	struct Sprite* parent; /**< parent sprite object */
	Pixel* tex;            /**< pixels */
} Frame;

/**
 * @brief sprite playing event callback functor
 *
 * @param[in] _cvs     - canvas object
 * @param[in] _spr     - sprite object
 * @param[in] _begin   - begin frame name of this animation
 * @param[in] _end     - end frame name of this animation
 * @param[in] _currIdx - current frame index
 * @return - execution status
 */
typedef s32 (* sprite_playing_callback_func)(struct Canvas* _cvs, struct Sprite* _spr, const Str _begin, const Str _end, s32 _currIdx);

/**
 * @brief time line structure
 */
typedef struct TimeLine {
	Str shape_file_name;                   /**< shape file name */
	Str brush_file_name;                   /**< brush file name */
	Str palete_file_name;                  /**< palete file name */
	Frame* frames;                         /**< all frames */
	s32 frame_count;                       /**< frames count */
	s32 current_frame;                     /**< current frame index */
	s32 last_frame;                        /**< last frame index */
	ht_node_t* named_frames;               /**< named frame information */
	Str begin_name;                        /**< begin frame name */
	Str end_name;                          /**< end frame name */
	s32 begin_index;                       /**< begin frame index */
	s32 end_index;                         /**< end frame index */
	bl pause;                              /**< whether paused */
	bl loop;                               /**< whether loop between begin and end frame */
	sprite_playing_callback_func callback; /**< sprite playing event callback functor */
} TimeLine;

/**
 * @brief customized animation structure
 */
typedef struct CustomAnimation {
	Ptr context;       /**< animation context */
	destroyer destroy; /**< destroyer functor */
} CustomAnimation;

/**
 * @brief sprite removing callback functor
 *
 * @param[in] _handlerObj - callback receiver object
 * @param[in] _cvs        - canvas object
 * @param[in] _spr        - removing sprite object
 */
typedef void (* sprite_removing_callback_func)(Ptr _handlerObj, struct Canvas* _cvs, struct Sprite* _spr);

/**
 * @brief sprite collision callback functor
 *
 * @param[in] _cvs - canvas object
 * @param[in] _spr - sprite object
 * @param[in] _px  - x position of collided pixel in sprite
 * @param[in] _py  - y position of collided pixel in sprite
 */
typedef void (* sprite_collision_callback_func)(struct Canvas* _cvs, struct Sprite* _spr, s32 _px, s32 _py);

/**
 * @brief sprite updating functor
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _spr         - sprite object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
typedef void (* sprite_update_func)(struct Canvas* _cvs, struct Sprite* _spr, s32 _elapsedTime);
/**
 * @brief sprite rendering functor
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _spr         - sprite object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
typedef void (* sprite_render_func)(struct Canvas* _cvs, struct Sprite* _spr, s32 _elapsedTime);
/**
 * @brief sprite colliding functor
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _spr         - sprite object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
typedef void (* sprite_collide_func)(struct Canvas* _cvs, struct Sprite* _spr, s32 _elapsedTime);

/**
 * @brief user defined data
 */
typedef struct Userdata {
	Ptr data;          /**< data */
	destroyer destroy; /**< data destroyer */
} Userdata;

/**
 * @brief sprite structure
 */
typedef struct Sprite {
	struct Canvas* owner;                         /**< owner canvas object */
	Str name;                                     /**< name */
	s32 visibility;                               /**< visibility */
	AgeParamSet* params;                          /**< parameter set */
	bl store_params;                              /**< whether store parameters to saved data file or not */
	Userdata userdata;                            /**< user defined data */
	Point position;                               /**< position */
	Point old_position;                           /**< old position */
	Point last_frame_position;                    /**< last frame position */
	s32 direction;                                /**< moving direction */
	Size frame_size;                              /**< size of each frame */
	TimeLine time_line;                           /**< time line data */
	CustomAnimation custom_animation;             /**< customized animation data */
	f32 frame_rate;                               /**< frame rate information */
	s32 frame_tick;                               /**< frame updating time tick count */
	sprite_removing_callback_func object_removed; /**< sprite removing callback */
	u32 physics_mode;                             /**< physics mode */
	sprite_collision_callback_func collided;      /**< collided physics callback */
	MessageMap message_map;                       /**< message processing map */
	control_proc control;                         /**< controlling functor, for motion controlling */
	sprite_update_func update;                    /**< updating functor, for animation controlling */
	sprite_render_func prev_render;               /**< fire rendering functor */
	sprite_render_func post_render;               /**< post rendering functor */
	sprite_collide_func collide;                  /**< colliding functor */
} Sprite;

/**
 * @brief running context structure
 */
typedef struct RunningContext {
	s32 last_elapsed_time; /**< elapsed time since last frame */
	u32 last_lparam;       /**< first param of last message */
	u32 last_wparam;       /**< second param of last message */
	Ptr last_extra;        /**< extra user defined data of last message */
	Color last_color;      /**< color value since last draw call */
} RunningContext;

/**
 * @brief canvas rendering functor
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
typedef void (* canvas_render_func)(struct Canvas* _cvs, s32 _elapsedTime);

/**
 * @brief canvas structure
 */
typedef struct Canvas {
	Str name;                       /**< name */
	AgeParamSet* params;            /**< parameter set */
	bl store_params;                /**< whether store parameters to saved data file or not */
	Size size;                      /**< canvas size */
	Pixel* pixels;                  /**< frame buffer */
	ht_node_t* sprites;             /**< alive sprite objects */
	s32 frame_rate;                 /**< canvas frame rate, in millisecond */
	RunningContext context;         /**< running context */
	Sprite** dropped_sprites;       /**< dropped sprites */
	s32 dropped_sprites_count;      /**< dropped sprites count */
	s32 dropped_sprites_size;       /**< dropped sprites buffer size */
	MessageMap message_map;         /**< message processing map */
	control_proc control;           /**< canvas controlling functor*/
	canvas_render_func prev_render; /**< fire rendering functor */
	canvas_render_func post_render; /**< post rendering functor */
} Canvas;

/**
 * @brief create a canvas object
 *
 * @param[in] _name - canvas name
 * @return - created canvas object
 */
AGE_API Canvas* create_canvas(const Str _name);
/**
 * @brief destroy a canvas object
 *
 * @param[in] _cvs - canvas object to be destroyed
 */
AGE_API void destroy_canvas(Canvas* _cvs);

/**
 * @brief set frame rate of a canvas
 *
 * @param[in] _cvs  - canvas object
 * @param[in] _rate - frame rate value
 */
AGE_API void set_frame_rate(Canvas* _cvs, s32 _rate);
/**
 * @brief get frame rate of a canvas
 *
 * @param[in] _cvs - canvas object
 * @return - frame rate value
 */
AGE_API s32 get_frame_rate(Canvas* _cvs);

/**
 * @brief run collition detection in a canvas
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
AGE_API void collide_canvas(Canvas* _cvs, s32 _elapsedTime);
/**
 * @brief update a canvas for a frame
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
AGE_API void update_canvas(Canvas* _cvs, s32 _elapsedTime);
/**
 * @brief delete dropped sprites
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
AGE_API void tidy_canvas(Canvas* _cvs, s32 _elapsedTime);
/**
 * @brief render a canvas for a frame
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
AGE_API void render_canvas(Canvas* _cvs, s32 _elapsedTime);

/**
 * @brief get a sprite by name
 *
 * @param[in] _cvs  - canvas object
 * @param[in] _name - sprite name
 * @return - corresponded sprite object, zero pointer for none
 */
AGE_API Sprite* get_sprite_by_name(Canvas* _cvs, const Str _name);
/**
 * @brief create a sprite object
 *
 * @param[in] _cvs        - canvas object
 * @param[in] _name       - sprite name
 * @param[in] _shapeFile  - shape data file name
 * @param[in] _brushFile  - brush data file name
 * @param[in] _paleteFile - palete data file name
 * @return - created sprite object
 */
AGE_API Sprite* create_sprite(Canvas* _cvs, const Str _name, const Str _shapeFile, const Str _brushFile, const Str _paleteFile);
/**
 * @brief create a new sprite object and copy data from an exists sprite object to it
 *
 * @param[in] _cvs     - canvas object
 * @param[in] _srcName - source sprite name
 * @param[in] _tgtName - target sprite name
 * @return - new created target sprite object
 */
AGE_API Sprite* clone_sprite(Canvas* _cvs, const Str _srcName, const Str _tgtName);
/**
 * @brief destroy a sprite in a canvas
 *
 * @param[in] _cvs - canvas object
 * @param[in] _spr - sprite object
 */
AGE_API void destroy_sprite(Canvas* _cvs, Sprite* _spr);
/**
 * @brief destroy all sprites in a canvas
 *
 * @param[in] _cvs - canvas object
 */
AGE_API void destroy_all_sprites(Canvas* _cvs);

/**
 * @brief get color of a pixel in a sprite
 *
 * @param[in] _cvs   - canvas object
 * @param[in] _spr   - sprite object
 * @param[in] _frame - frame index
 * @param[in] _x     - x offset of a pixel
 * @param[in] _y     - y offset of a pixel
 * @return - color of an appointed pixel
 */
AGE_API Color get_sprite_pixel_color(Canvas* _cvs, Sprite* _spr, s32 _frame, s32 _x, s32 _y);
/**
 * @brief set color of a pixel in a sprite
 *
 * @param[in] _cvs   - canvas object
 * @param[in] _spr   - sprite object
 * @param[in] _frame - frame index
 * @param[in] _x     - x offset of a pixel
 * @param[in] _y     - y offset of a pixel
 * @param[in] _col   - color of the appointed pixel
 */
AGE_API void set_sprite_pixel_color(Canvas* _cvs, Sprite* _spr, s32 _frame, s32 _x, s32 _y, Color _col);

/**
 * @brief set visibility of a sprite
 *
 * @param[in] _cvs - canvas object
 * @param[in] _spr - sprite object
 * @param[in] _vis - visible property
 * @return - return TRUE if succeed, or FALSE if failed
 */
AGE_API bl set_sprite_visible(Canvas* _cvs, Sprite* _spr, bl _vis);
/**
 * @brief set visibility of a sprite
 *
 * @param[in] _cvs  - canvas object
 * @param[in] _spr  - sprite object
 * @param[out] _vis - visible property pointer
 * @return - return TRUE if succeed, or FALSE if failed
 */
AGE_API bl get_sprite_visible(Canvas* _cvs, Sprite* _spr, bl* _vis);

/**
 * @brief set position of a sprite
 *
 * @param[in] _cvs - canvas object
 * @param[in] _spr - sprite object
 * @param[in] _x   - x
 * @param[in] _y   - y
 * @return - return TRUE if succeed, or FALSE if failed
 */
AGE_API bl set_sprite_position(Canvas* _cvs, Sprite* _spr, s32 _x, s32 _y);
/**
 * @brief get position of a sprite
 *
 * @param[in] _cvs - canvas object
 * @param[in] _spr - sprite object
 * @param[out] _x  - pointer to output x
 * @param[out] _y  - pointer to output y
 * @return - return TRUE if succeed, or FALSE if failed
 */
AGE_API bl get_sprite_position(Canvas* _cvs, Sprite* _spr, s32* _x, s32* _y);

/**
 * @brief get index of a given named frame
 *
 * @param[in] _cvs  - canvas object
 * @param[in] _spr  - sprite object
 * @param[in] _name - frame name
 * @return - found index, INVALID_FRAME_INDEX if not found
 */
AGE_API s32 get_named_frame_index(Canvas* _cvs, Sprite* _spr, const Str _name);
/**
 * @brief play an animation of a time line
 *
 * @param[in] _cvs   - canvas object
 * @param[in] _spr   - sprite object
 * @param[in] _begin - begin frame name of this animation
 * @param[in] _end   - end frame name of this animation
 * @param[in] _loop  - whether loop this animation
 * @param[in] _cb    - playing event callback functor
 * @return - return TRUE if succeed, or FALSE if failed
 */
AGE_API bl play_sprite(Canvas* _cvs, Sprite* _spr, const Str _begin, const Str _end, bl _loop, sprite_playing_callback_func _cb);
/**
 * @brief pause an animation of a time line
 *
 * @param[in] _cvs - canvas object
 * @param[in] _spr - sprite object
 * @return - return TRUE if succeed, or FALSE if failed
 */
AGE_API bl pause_sprite(Canvas* _cvs, Sprite* _spr);
/**
 * @brief resume an animation of a time line
 *
 * @param[in] _cvs - canvas object
 * @param[in] _spr - sprite object
 * @return - return TRUE if succeed, or FALSE if failed
 */
AGE_API bl resume_sprite(Canvas* _cvs, Sprite* _spr);
/**
 * @brief stop an animation of a time line
 *
 * @param[in] _cvs    - canvas object
 * @param[in] _spr    - sprite object
 * @param[in] _stopAt - stop animation at which frame, pass INVALID_FRAME_INDEX to stop at current frame
 * @return - return TRUE if succeed, or FALSE if failed
 */
AGE_API bl stop_sprite(Canvas* _cvs, Sprite* _spr, s32 _stopAt);

/**
 * @brief sprite updating
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _spr         - sprite object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
AGE_API void update_sprite(Canvas* _cvs, Sprite* _spr, s32 _elapsedTime);
/**
 * @brief prev sprite rendering
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _spr         - sprite object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
AGE_API void prev_render_sprite(Canvas* _cvs, Sprite* _spr, s32 _elapsedTime);
/**
 * @brief post sprite rendering
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _spr         - sprite object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
AGE_API void post_render_sprite(Canvas* _cvs, Sprite* _spr, s32 _elapsedTime);
/**
 * @brief sprite colliding
 *
 * @param[in] _cvs         - canvas object
 * @param[in] _spr         - sprite object
 * @param[in] _elapsedTime - elapsed time since last frame
 */
AGE_API void collide_sprite(Canvas* _cvs, Sprite* _spr, s32 _elapsedTime);

/**
 * @brief get physics mode of a sprite object
 *
 * @param[in] _cvs - canvas object
 * @param[in] _spr - sprite object
 * @return - physics mode
 */
AGE_API u32 get_sprite_physics_mode(Canvas* _cvs, Sprite* _spr);
/**
 * @brief set physics mode of a sprite object
 *
 * @param[in] _cvs  - canvas object
 * @param[in] _spr  - sprite object
 * @param[in] _mode - physics mode
 */
AGE_API void set_sprite_physics_mode(Canvas* _cvs, Sprite* _spr, u32 _mode);

/**
 * @brief draw a string on a canvas
 *
 * @param[in] _cvs  - canvas object
 * @param[in] _font - drawing font
 * @param[in] _x    - x position to draw
 * @param[in] _y    - y position to draw
 * @param[in] _text - text to draw
 */
AGE_API void draw_string(Canvas* _cvs, Font* _font, s32 _x, s32 _y, const Str _text, ...);
/**
 * @brief draw a charactor on a canvas
 *
 * @param[in] _cvs  - canvas object
 * @param[in] _font - drawing font
 * @param[in] _x    - x position to draw
 * @param[in] _y    - y position to draw
 * @param[in] _ch   - charactor to draw
 */
AGE_API void put_char(Canvas* _cvs, Font* _font, s32 _x, s32 _y, s8 _ch);

/**
 * @brief get a mapped color value
 *
 * @param[in] _index - color index
 * @return - mapped color value
 */
AGE_API Color get_mapped_color(s32 _index);
/**
 * @brief set whether the console cursor is visible
 *
 * @param[in] _cvs - canvas object
 * @param[in] _vis - whether the console cursor is visible
 */
AGE_API void set_cursor_visible(Canvas* _cvs, bl _vis);
/**
 * @brief set console cursor posotion to a given place
 *
 * @param[in] _cvs - canvas object
 * @param[in] _x   - x
 * @param[in] _y   - y
 */
AGE_API void goto_xy(Canvas* _cvs, s32 _x, s32 _y);
/**
 * @brief set console output color
 *
 * @param[in] _cvs - canvas object
 * @param[in] _col - console output color
 */
AGE_API void set_color(Canvas* _cvs, Color _col);

/**
 * @brief clear a pixel
 *
 * @param[in] _cvs - canvas object
 * @param[in] _x   - x position of the pixel to be cleared
 * @param[in] _y   - y position of the pixel to be cleared
 */
AGE_API void clear_pixel(Canvas* _cvs, s32 _x, s32 _y);
/**
 * @brief clear screen
 *
 * @param[in] _cvs - canvas object
 */
AGE_API void clear_screen(Canvas* _cvs);

#endif /* __AGE_RENDERER_H__ */

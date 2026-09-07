# OPL FUTURE UI

Futuristic premium frontend for the PS2 OPL Refactored project.

## Concept

Dark, high-contrast "next-gen HUD" style: deep navy/black background, cyan (`#00D8FF`) neon accents,
a cover-art carousel as the centerpiece, a bottom device selector dock, and a top status bar.
Layout coordinates use OPL's logical 640x480 space and are resolution-independent.

## New Theme Elements (src/themes.c)

Themes are fully data-driven through `misc/conf_theme_OPL.cfg`. Four new element types were added:

### ELEM_TYPE_CAROUSEL - `carousel_t`
- Horizontal cover-art carousel centered on a main cover, with smaller scaled side covers
- Keys: `_pattern` (texture cache suffix, e.g. `COV`), `_count`, `_default`, `_neighbors`, `_spacing`, `_side_scale`
- `aligned=1`: `posX/posY` = center of the main cover (ItemCover convention)
- Drawing: gradient glow (`rmDrawRectVGrad`) behind main cover, neon thin frame (`rmDrawFrame`), main cover on top
- Neighbors drawn far→near so the main (active) cover is always on top
- Falls back to `default` texture; empty/missing art handled gracefully
- Shares texture caches via `findDuplicate`; never steals another element's cache

### ELEM_TYPE_SELECTOR_BAR - `selector_bar_t`
- Bottom dock listing the device/mode pipeline (e.g. `HDD * Ethernet * USB * MX4SIO * APPS`)
- Keys: `_devices` (comma-separated labels drawn as pills)
- Evenly divides the configured width; auto-sized to content when `width` is unset
- Detects visible devices at runtime (`menuHeadToTail`) and adds/disables entries accordingly
- Active mode: gradient fill in theme highlight colors + neon cyan border + animated status icon
- Inactive modes: translucent fill + text color + device status dot
- Icons drawn from theme texture cache (`thmGetTexture`)

### ELEM_TYPE_STATUS_BAR - `status_bar_t`
- Slim HUD line (default top-center) summarizing device status
- Two-pass layout: measure each device string, add separators, center the whole line
- Per BDM device prints localized name + READY/STANDBY status
- Ethernet device name + ONLINE/OFFLINE status text
- Keys: `_devices` (comma-separated)

### ELEM_TYPE_FRAME - `frame_t`
- Decorative neon frame: 4 corner brackets + 4 edge lines + translucent inner tint
- Keys: `_thickness` (edge thickness), `_size` (corner bracket length), `_color` (`#RRGGBB`)
- Draw order: background tint -> bracket corners -> edge lines between brackets

## Layout: Main Screen

```
┌────────────────────────────────────────────────────────┐
│ [icon]  < DEVICE NAME >                    [### GAMES] │
│            STATUS BAR (device statuses)                │
│  ┌────────────────────────┐        ┌───────────────┐   │
│  │                        │        │  NEON FRAME   │   │
│  │      GAMES LIST        │        │   [COVER]     │   │
│  │  ┌──────────────────┐  │        │   [COVER]     │   │
│  │  │▶ selected game   │  │        │   [COVER]     │   │
│  │  └──────────────────┘  │        └───────────────┘   │
│  │                        │        « CANVAS / ITEM »    │
│  │                        │                            │
│  │                        │        ═══ BDM ════════   │
│  ════════════ BDM ════════    !                      │
│  HDD ▪ Ethernet ▪ USB ▪ MX4SIO ▪ APPS  (SELECTOR BAR) │
│                        hints  ⌂⏎🖭⬆⬇                  │
└────────────────────────────────────────────────────────┘
```

- `main1` MenuIcon top-left, `main2` MenuText (device) top-center, `main3` StatusBar below
- `main4` Frame wraps the carousel area, `main5` Carousel (main cover center x=430,y=210, 180x180)
- `main6` ItemText under the cover, `main7` ItemsList left column (14 visible, paginated)
- `main8` SelectorBar bottom dock (600px wide, centered at y-30)
- `main9` HintText above dock (keyboard hints follow OPL defaults)
- `main10` GameCountText top-right, `main11` BdmIndex centered low, `main12` LoadingIcon bottom-right

## Layout: Apps Screen

Reuses main elements; apps-specific overrides:
- `appsMain5` ItemCover (covers + `apps_case` overlay in the classic OPL style) instead of the Carousel
- `appsMain7` ItemsList at the same position as games list

## Layout: Info Screen

- `info1` StaticImage background, `info2-6` title/genre/release/developer/size attributes
- `info7` Description (wrap width 300), `info8-12` media/format/vmode/aspect/scan badges row
- `info13` Rating, `info14` Device, `info15/16` screenshots (SCR/SCR2) right column
- `info17` Frame panel wraps the right details column
- `info18` InfoHintText bottom center

## Element Behaviors (reference)

- `initMutableImage` reads `_pattern`/`_count` for GAME_IMAGE types and shares caches/clutches via `findDuplicate`
- Coordinate convention: `width/height = DIM_INF` = natural texture size, `SCALING_RATIO` preserves aspect
- Negative coordinates relative to screen: `x<0 → screenWidth + x`, `y<0 → (screenHeight + y)*usedHeight/480`
- Colors `#RRGGBB` only (6 hex digits) via `configGetColor`
- `fntRenderString` `y` is the baseline; ALIGN_HCENTER + width=0 centers the whole string

## Build

```sh
make all NOT_PACKED=1 PS2_PACKER_DIR=/usr/local/ps2dev/bin        # opl.elf (unpacked)
mips64r5900el-ps2-elf-strip -o opl_stripped.elf opl.elf            # stripped
/usr/local/ps2dev/bin/ps2-packer opl_stripped.elf OPNPS2LD.ELF     # packed (absolute paths)
```

> Makefile packer target (line 387) fails because `cd $(PS2_PACKER_DIR)` breaks the relative `$<`.
> Run `ps2-packer` manually with absolute paths (above).

## Artifacts

- `opl.elf` ~10,335,276 B (uncompressed)
- `opl_stripped.elf` ~3,242,676 B
- `OPNPS2LD.ELF` ~1,346,836 B (ratio 58.47%)

## Testing

- Compiles cleanly (GCC 15.2.0, no warnings in themes.c; only pre-existing LOG redefine in ioman.h)
- Needs PS2 hardware runtime test: covers, carousel neighbors, selector bar modes, status dots, pagination, info screen

## Design Notes

- Zero new APIs beyond the theming system - everything is config-driven
- All new elements build on existing `renderman` drawing primitives
- Colors picked for CRT/video-out readability (high contrast, saturated cyan accent)
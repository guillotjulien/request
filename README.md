# Request

Nothing to see here!

## Building

- `meson build`
- `ninja -C build`
- `./build/src/request`

## Format

`find src/ \( -name "*.c" -o -name "*.h" \) -exec uncrustify -c uncrustify.cfg --no-backup --if-changed {} +`
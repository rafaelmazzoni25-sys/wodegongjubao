# MuExporter

MuExporter is a standalone command-line tool that demonstrates how a MuOnline scene can be
loaded through a lightweight plugin description, transformed into an in-memory world
representation, and exported into a portable JSON snapshot. The implementation is entirely
self-contained and does not reuse any source files from the original editor codebase.

## Features

* Discover simple plugin descriptors from a directory to determine which importer handles a
  given map file format.
* Parse a minimal MuOnline-inspired terrain and object format (``.scene``) implemented by the
  built-in SoulScene importer.
* Build an in-memory scene graph containing terrain tiles and placed objects.
* Export the resulting scene into JSON, including terrain layout, heights, and object metadata.

## Building

The project can be built with any C++20 compiler. A convenience CMake configuration is provided.

```bash
cd MuExporter
cmake -S . -B build
cmake --build build
```

This produces the ``muexporter`` executable in ``build`` (location depends on your generator).

### Using it outside this repository

MuExporter does not depend on any other code in the ``wodegongjubao`` tree. To turn it into a
completely standalone project, copy or move the entire ``MuExporter`` directory into an empty
workspace (or clone it into a fresh repository) and run the same CMake commands shown above. The
``data`` directory is optional, but keeping it alongside the sources provides the sample plugin and
scene file referenced in the usage examples.

## Usage

MuExporter expects three inputs: a directory containing plugin descriptors, a directory with map
files, and the name of the map file to export. The tool writes the exported JSON to a file or to
stdout when ``--output`` is omitted.

```bash
./muexporter \
  --plugins data/plugins \
  --maps data/maps \
  --map devias.scene \
  --output devias.json
```

### Sample Data

The repository ships with a minimal plugin descriptor (``soul_scene.plug``) and a sample
``devias.scene`` map file illustrating how the importer understands terrain and object data. You
can inspect and modify these files to experiment with the exporter.

## Scene File Format

The custom ``.scene`` format is made of INI-like sections:

* ``[scene]`` – high level metadata such as the map name and version.
* ``[terrain]`` – grid dimensions, cell size, and flattened height samples.
* ``[objects]`` – a list of terrain objects specified via ``object=`` lines.

Blank lines and comments beginning with ``#`` are ignored.

## License

This sample is provided for demonstration purposes and does not include any proprietary MuOnline
assets.

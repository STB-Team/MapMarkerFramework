# Custom Map Marker Framework
This SKSE plugin provides a framework for mods to add new map marker icons without being subject to
the limitations that normally apply when adding markers without SKSE. This means:
- No conflicts with mods that edit the UI
- No conflicts between mods that add markers
- Customizable discovery music
- Far more than 256 icons

The canonical implementation of this framework, which it was created for, is
[CoMAP - Common Marker Addon Project](https://www.nexusmods.com/skyrimspecialedition/mods/56123) by
Jelidity.

## Usage
The SKSE plugin reads config files from the `Data\MapMarkers\` directory in order to apply its
changes. Each active plugin in your load order can have a config file associated with it, using the
same stem filename and a .json extension. For example, the config file corresponding to the
Skyrim.esm plugin is `Data\MapMarkers\Skyrim.json`. Conflicts between these config files are
resolved based on the plugin load order, just like plugin contents, .bsa files, and .ini files.

### Adding New Markers
In order to add new icons, you first need to create a .swf file that exports 2 sprites, one for
the discovered marker and one for the undiscovered marker. See the
[CoMAP Dev Kit Guide](https://docs.google.com/document/d/1MNDihv3ew4MoghlMQNZhYmYi12yHz0_ZMUJQ2w_uOUE/edit)
for information on how to create the icons and .swf files.

Here is an example of what a config that assigns a new marker to Riverwood could look like:
```jsonc
{
  "iconDefinitions": [
    {
      "name": "RiverwoodMarker",
      "source": {
        "path": "resources\\mymarkerart.swf",
        "exportNames": ["RiverwoodMarker", "RiverwoodMarkerUndiscovered"]
      },
      "scale": 1.333333,
      "discoveryMusic": "MUSDiscoveryTown"
    }
  ],
  "mapMarkers": [
    {
      "refID": "Skyrim.esm|162A4",
      "iconName": "RiverwoodMarker"
    }
  ]
}
```
The `name` property is a name that you provide for each icon so that it can be assigned to via the
`iconName` property. It is recommended to add a mod-specific prefix to your names to avoid
conflicts, just like with editor IDs, scripts, and asset paths.

The `source` property contains information about the .swf file where markers should be read from.
It is recommended to use the `resources\` subdirectory for your resources for organizational
purposes. The export names are defined within the .swf file. You should provide exactly 2 names,
corresponding to the discovered and undiscovered marker.

The `scale` property configures how large the icon should appear on the map. Recommended values are
`1.333333` for towns and cities, and `0.666667` for minor locations.

The `discoveryMusic` property configures the music that plays when the player first discovers the
location. This property accepts the Editor ID of a Music Type form. The tracks provided by the
vanilla game are `MUSDiscoveryCity`, `MUSDiscoveryTown`, `MUSDiscoveryDungeon`, and
`MUSDiscoveryGeneric`.

The `hideFromHUD` property can also be added and set to `true` for markers that should not appear on
the HUD compass. This is useful for icons such as arrows which would not make sense in the HUD.

### Editing Existing Markers
It is also possible to make some edits to existing markers within the config.

Here is an example of some possible edits:
```jsonc
{
  "iconDefinitions": [
    {
      "index": 15, // Mine
      "discoveryMusic": "MUSDiscoveryGeneric"
    },
    {
      "index": 57, // DLC02ToSkyrim
      "hideFromHUD": true
    },
    {
      "index": 58, // DLC02ToSolstheim
      "hideFromHUD": true
    },
  ]
}
```
The `index` property identifies the icon by index. These can be found by decompiling the relevant
.swf files or in the
[CommonLibSSE source code](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/E/ExtraMapMarker.h).

The `discoveryMusic` and `hideFromHUD` properties are the same as for new markers. Note that the
`scale` property is NOT supported when editing existing icons.

### Local Map Markers
You can customize the markers that appear for doors on the local map.

Here is an example of a possible local map config:
```jsonc
{
  "mapMarkers": [
    {
      "locType": "LocTypeInn",
      "iconName": "InnMarker"
    },
    {
      "vendorList": "Skyrim.esm|937A0", // VendorItemsTailor
      "iconName": "TailorMarker"
    },
    {
      "location": "Skyrim.esm|2263B", // Riften Fishery
      "iconName": "FishMarker"
    }
  ]
}
```
The `location` property specifies a specific location to assign an icon to.

The `vendorList` property specifies a vendor's buy/sell list. The specified icon will be used for
any location containing a vendor that uses that list.

The `locType` property specifies a location keyword, by name. The specified icon will be used for
any location that uses that keyword.

Each of the above properties take priority from least to most specific. `location` has the highest
priority, followed by `vendorList`, and then `locType`.

## STB: runtime undiscovered icons (2.2.2.1)

This STB distribution targets **Skyrim SE 1.5.97 only**, not AE. It retains the
upstream 2.2.2 framework but builds against the pinned STB-Team CommonLib SE.
`SKSEPlugin_Query` rejects other runtimes before any SE relocation is used.
The original SE 2.2.1 DLL/PDB and the local 1.5.97 executable were used to verify:
HUD `50716+0xFF`, map `52206+0x1CF`, local-map door `52081+0x7B5`,
discovery music `50758+0x3B3` (resume `+0x51D`, `rdi+0x44`, `rbp-0x29`),
GFx string conversion `80274`, and the Scaleform vtables in `Offset.Ext.h`.
The call targets are `80302` (HUD/map) and `52190` (local-map door).

The STB fork adds `MMF_RequestIconModeAPI(1)` (see `include/MMF/IconModeAPI.h`).
`SetUndiscoveredIcons(bool)` accepts calls from any thread and applies the latest
request on SKSE's Scaleform UI queue. The default is `false`.

Configs opt in with a root-level `"revealUndiscoveredPrefix": "Undiscovered_Tir"`.
Only definitions whose second `exportNames` entry starts with this nonempty prefix
participate. In icon mode they use the first export instead; question mode restores
the original second export. Discovered markers, nonmatching definitions, scale,
music, `hideFromHUD`, and game discovery/visibility flags are unchanged.
The original MMF read-only API is unchanged.

Both tag lists are prepared once per loaded sprite. Switching selects existing
lists rather than inserting frames again. The visible HUD markers are redrawn;
map markers reflect the setting on the next map opening. Cached map/HUD resources
are recognized so reopening a menu does not shift frame indices again.
At most the latest HUD and map resource owners are retained.
The upstream `bObscuredUndiscovered=1` option takes precedence over this extension.

STB exposes the switch under **STB / Эффекты / Карта и компас** and saves
`ShowMarkerIcons` in `Data/SKSE/Plugins/UserSettingsSTB.json` (`0` = questions,
`1` = icons). No config files are rewritten at runtime. Remove the obsolete
`STB Show Marker Icons/MapMarkers/Skyrim.json` override when migrating.

`dist/Data/MapMarkers` contains the STB definitions, bindings and their resources.
The migration is equivalent to the former override for all 423 participating
definitions; 66 other definitions are preserved.

## Build Instructions
### Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2026](https://visualstudio.microsoft.com/)
	* Desktop development with C++

### Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

### Building
```
git clone --recurse-submodules -b feature/stb-marker-mode https://github.com/STB-Team/MapMarkerFramework
cd MapMarkerFramework
cmake --preset vs2026-windows -DSKYRIM_SUPPORT_AE=OFF
cmake --build build --config Release
```

# Unvirt Command Reference

## File Operations

### Load File

Syntax: `load [deep | shallow] <filepath>`

Description: Load a Virtools composition.

Arguments:
- `[deep | shallow]`: The stage of loading. 
  * `shallow` will load to CKStateChunk stage.
  * `deep` will load to CKObject stage.
- `<filepath>`: The path to loaded file.

### Unload File

Syntax: `unload`

Description: Release loaded Virtools composition.

### Save File

Syntax: `save <filepath>`

Description: Save the loaded file into a new file.

Arguments:
- `<filepath>`: The path to saved file.

## List & Search

### Show Header Info

Syntax: `info`

Description: Show the header infomation of loaded Virtools composition.

### List Objects, Managers or Search Result

Syntax: `ls [obj | mgr | search] <page>`

Description: List objects, managers of loaded Virtools composition,
or the search result of them.

Arguments:
- `[obj | mgr | search]`: Which one you want to list.
- `<page>`: The page index. Start with 1.

### Show Object or Manager Data

Syntax: `data [obj | mgr] <index>`

Description: Show the data of specified object or manager.

Arguments:
- `[obj | mgr]`: Which one you want to show data.
- `<index>`: The index of object or manager which can be fetched by list command. Start with 0.

### Show State Chunk Data

Syntax: `chunk [obj | mgr] <index>`

Description: Show the specific CKStateChunk data.

Arguments:
- `[obj | mgr]`: Which one you want to show chunk
- `<index>`: The index of object or manager which can be fetched by list command. Start with 0.

### Search Objects or Managers

Syntax: `search [obj | mgr] [plain | re] <text>`

Description: Search the name of object or manager= by plain text or regex. 
Please note that the regex have limited UTF8 support and may cause undefined behavior.

Arguments:
- `[obj | mgr]`: Which one you want to search.
- `[plain | re]`: The search mode.
  * `plain` will search by plain string. It simply check whether name has your given substring.
  * `re` will do regex search.
- `<text>`: The plain text or regex to search.

## Display

### Page Item Count

Syntax: `items <count>`

Description: Set up how many items should be listed in one page when using 'ls' command.

Arguments:
- `<count>`: The count of items you want to show in one page

### Display Style

Syntax: `style [full | simple]`

Description: Change the detail level of shown data in `ls` command.

Arguments:
- `[full | simple]`: The style of shown content.
  * `full` will show more detailed information.
  * `simple` will show less information.

## Environment

### Set Encoding

Syntax: `encoding <enc>`

Description: Set the encoding series for Virtools composition loading ans saving.

Arguments:
- `<enc>`: CKContext used encoding separated by ','. Support multiple encoding.

### Temp Path

Syntax: `temp <temppath>`

Description: Set the Temp path for CKContext.

Arguments:
- `<temppath>`: The path to Temp folder

### Data Resource Path

#### Clear Data Resource Path

Syntax: `rsc clear`

Description: Clear all data resources paths.

#### Add Data Resource Path

Syntax: `rsc add <datares>`

Description: Manage data resources paths.

Arguments:
- `<datares>`: The data resources path.

## System

### Debug Test

Syntax: `test`

Description: Call custom debugging function (only available in Debug mode).

### Show Version

Syntax: `version`

Description: Show the version info about this program.

### Show Help

Syntax: `help`

Description: Output this help page.

### Exit Program

Syntax: `exit`

Description: Exit program.

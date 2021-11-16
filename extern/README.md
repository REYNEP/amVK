## TODO: Use SCons [like Godot]

## Only supposed to be used as subdirectory `extern` of `amVK`.... we set up a submodule flow
### Remember not to use  `submodule recurse`.... cz `inc` & `lib` has it all.... 

###### Currently lib folder also will have .exe   as   set_target_properties(... RUNTIME_OUTPUT_DIRECTORY ${_amVK_EXTERN_LIB}) is set
% RELOAD_WIZARD reloads an Opticks wizard file (*.wiz) from disk.
%   RELOAD_WIZARD(X) refreshes the Opticks wizard file X from the disk.
%
%   Once a wizard has been loaded, it will remain in memory until RELOAD_WIZARD
%   is called or the plug-in is unloaded. A wizard previously loaded may have
%   been modified. To force a wizard to be reloaded from disk, use RELOAD_WIZARD.
%
%   See also GET_METADATA, SET_METADATA, EXECUTE_WIZARD.
lasterr('This command must be executed from Opticks.')

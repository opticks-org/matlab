% EXECUTE_WIZARD runs an Opticks wizard file (*.wiz).
%   EXECUTE_WIZARD(X, Y, Z) runs the Opticks wizard file X such that
%      X is in batch mode if Y indicates 't', 'true', 1, or a similar value.
%      X displays progress if Z indicates 't', 'true', 1, or a similar value.
%
%   Once a wizard has been loaded, it will remain in memory until RELOAD_WIZARD
%   is called or the plug-in is unloaded. A wizard previously loaded may have
%   been modified. To force a wizard to be reloaded from disk, use RELOAD_WIZARD.
%
% See also GET_METADATA, SET_METADATA, RELOAD_WIZARD.
lasterr('This command must be executed from Opticks.')

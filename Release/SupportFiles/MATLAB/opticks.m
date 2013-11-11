% OPTICKS Lists all commands specific to Opticks.
%   These commands are processed by Opticks and cannot be executed from MATLAB
%   or while doing any type of branching or looping such as an 'if' because the
%   Opticks/MATLAB interface must buffer commands from Opticks to send to
%   MATLAB while performing branching or looping, and these commands cannot be
%   reliably executed while buffering.
%
%   While branching or looping, all commands are executed before any output is
%   displayed. This happens because the entire command must be buffered by
%   Opticks prior to being sent to MATLAB for processing. Because of this, any
%   intermediate output (e.g.: printing progress to the screen) will not appear
%   until after the command has executed in its entirety. An alternative is to
%   use the built-in MATLAB function waitbar to report progress.
%
%   In addition, certain syntax (e.g.: including multiple statements in a single
%   line and the ellipses operator) is also not fully supported. When using
%   Opticks commands, please separate multiple statements such that each one
%   appears on its own line.
%
%   All Opticks commands are case sensitive for all versions of MATLAB.
lasterr('This command must be executed from Opticks.')
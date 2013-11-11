% SET_WINDOW_POSITION sets the position of the specified window.
%   SET_WINDOW_POSITION(X, Y, Z, W) sets the position of the window named Z
%   which should be of type W to the value of (X, Y) where (0, 0) is the top
%   left of the Opticks workspace with positive x pointing to the right of the
%   screen and positive y pointing to the bottom of the screen.
%
%   The default values are
%      Z = the name of the active workspace window
%      W = 'SpatialDataWindow' but other valid values are:
%        'WorkspaceWindow'
%        'ProductWindow'
%        'DockWindow'
%        'PlotWindow'
%
%   The value of Z is case sensitive.
%   The value of W is not case sensitive.
lasterr('This command must be executed from Opticks.')

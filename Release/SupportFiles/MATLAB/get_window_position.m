% GET_WINDOW_POSITION is the position of the specified window.
%   GET_WINDOW_POSITION(Z, W) gets the position of the window named Z
%   which should be of type W where (0, 0) is the top left of the Opticks
%   workspace with positive x pointing to the right of the screen and positive
%   y pointing to the bottom of the screen.
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
%
%   Note: If no variable is specified on the left-hand side of the equation,
%   the variable 'pos_x' is used to denote the x-position of the window, and
%   the variable 'pos_y' is used to denote the y-position of the window.
lasterr('This command must be executed from Opticks.')

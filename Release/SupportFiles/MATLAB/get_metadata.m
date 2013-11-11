% GET_METADATA is a stringified value from an Opticks element or wizard.
%   GET_METADATA(X, Y, Z, W) does the following:
%      If W is an empty string:
%         The metadata from element X is queried for the value Y.
%         If this value can be represented as a string, the result is stored in
%         the MATLAB workspace as Z.
%      If W is not an empty string:
%         X is ignored.
%         The wizard file W is loaded.
%         The wizard object value Y is queried in the wizard W.
%         If this value can be represented as a string, the result is stored in
%         the MATLAB workspace as Z.
%
%   The default is W = empty string, indicating that no wizard should be used.
%
%   See also SET_METADATA, RELOAD_WIZARD.
lasterr('This command must be executed from Opticks.')

% Test suite of Opticks/MATLAB interface commands
fprintf('Testing MATLAB/Opticks Interface. . .\n')
fprintf('Several windows should appear and disappear during the course of this test.\n')

% Assumes that the "Confirm Close" Workspace Window option is NOT set (i.e.: windows will close without a prompt).
cc = get_configuration_setting('WorkspaceWindow/ConfirmClose');
if strcmp(cc, 'False') ~= 1
   fprintf('   The WorkspaceWindow/ConfirmClose option is set to true.\nPlease disable it and try again.\n')
end

% Basic Tests
A = zeros(2, 2, 2);
A(:,:,1) = [1, 2; 3, 4];
A(:,:,2) = [5, 6; 7, 8];
array_to_opticks('A')
array_to_matlab()
if raster ~= A
   fprintf('   Array A does not match after being passed to Opticks\n')
end
close_window()

% Test interleave conversion for BIP.
B = zeros(2, 3, 4);
B(:,:,1) = [1, 2 , 3; 4 , 5, 6];
B(:,:,2) = [7, 8 , 9; 10, 11, 12];
B(:,:,3) = [13, 14, 15; 16, 17, 18];
B(:,:,4) = [19, 20, 21; 22, 23, 24];
array_to_opticks('B', '', 1, 1, 'bip')
array_to_matlab()
if raster ~= B
   fprintf('   Array B does not match after being passed to Opticks\n')
end
close_window()

% Test interleave conversion for BIL.
C = B;
array_to_opticks('C', '', 1, 1, 'bil')
array_to_matlab()
if raster ~= C
   fprintf('   Array C does not match after being passed to Opticks\n')
end
close_window()

% Most of the other internal commands require a dataset to be present in Opticks, so create one now.
% This is a larger version of the same data used for diag_test (a diagonal line animating across the image from the top left to the bottom right).
rows = 256;
cols = 256;
bands = 256;
test = zeros(rows, cols, bands);
for z = 1:bands
   test(:,:,z) = [eye(z, cols); zeros(rows - z, cols)];
end

% Test ArrayFromMatlabCommand
array_to_opticks('test');

% Test ArraySizeCommand
sz = array_size();
if sz ~= size(test)
   fprintf('   Error with array_size command.\n')
end

% Test Animation Commands.
% CreateAnimationCommand
create_animation();

% GetAnimationCycleCommand
% SetAnimationCycleCommand
x = 'play once';
set_animation_cycle(x);
y = get_animation_cycle();
if strcmpi(x, y) ~= 1
   fprintf('   Error with get_animation_cycle command for value %s.\n', x)
end

x = 'BOUNCE';
set_animation_cycle(x);
y = get_animation_cycle();
if strcmpi(x, y) ~= 1
   fprintf('   Error with get_animation_cycle command for value %s.\n', x)
end

x = 'rePeAt';
set_animation_cycle(x);
y = get_animation_cycle();
if strcmpi(x, y) ~= 1
   fprintf('   Error with get_animation_cycle command for value %s.\n', x)
end

% GetIntervalMultiplierCommand
% SetIntervalMultiplierCommand
x = 30;
set_interval_multiplier(x);
y = get_interval_multiplier();
if x ~= y
   fprintf('   Error with get_interval_multiplier or set_interval_multiplier command.\n')
end

% Test LayerCommands.
% Test GetLayerNameCommand
x = get_layer_name(-1);
if strcmp('test', x) ~= 1
   fprintf('   Error with get_layer_name(-1) command.\n')
end

x = get_layer_name(0);
if strcmp('test', x) ~= 1
   fprintf('   Error with get_layer_name(0) command.\n')
end

% Test GetNumLayersCommand
x = get_num_layers();
if x ~= 1
   fprintf('   Error with get_num_layers command. x == %d\n', x)
end

% Test GetWindowLabelCommand
x = get_window_label();
if strcmp('test', x) ~= 1
   fprintf('   Error with get_window_label command.\n')
end

% Test SetWindowLabelCommand
x = 'another test';
set_window_label(x);
y = get_window_label();
if strcmp(x, y) ~= 1
   fprintf('   Error with get_window_label or set_window_label command.\n')
end

% Close the window.
close_window();

fprintf('Finished running opticks_test.')

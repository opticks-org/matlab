% Tests a variety of functions.
fprintf('Running diag_test. . .\n');
fprintf('   A window should appear with an animation of a thin dotted line along the main diagonal.\n');

rows = 60;
cols = 60;
bands = 60;
diag_test_window = zeros(rows, cols, bands);
for z = 1:bands
   diag_test_window(:,:,z) = [eye(z, cols); zeros(rows - z, cols)];
end

array_to_opticks('diag_test_window');
create_animation();
set_stretch_units('raw');
set_stretch_values(1, 0);
set_interval_multiplier(10);
set_animation_cycle('play_once');
set_animation_state('play_forward');

% There is no good way to wait for the animation to finish, so tell the user to close the window manually.
fprintf('   The window can be safely closed at any time.\n');
fprintf('Finished running diag_test.')

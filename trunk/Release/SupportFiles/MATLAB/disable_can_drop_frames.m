% DISABLE_CAN_DROP_FRAMES forces an Opticks animation controller to slow down.
%   DISABLE_CAN_DROP_FRAMES(X) forces the animation controller named X to slow
%   down when necessary so that no frames are skipped during playback.
%
%   The default value is X = the active animation controller.
%
%   Example:
%      >> rows = 60;
%      >> cols = 60;
%      >> bands = 60;
%      >> A = zeros(rows, cols, bands);
%      >> for z = 1:bands
%      >>    A(:,:,z) = [eye(z, cols); zeros(rows - z, cols)];
%      >> end
%      >> array_to_opticks('A')
%      >> create_animation()
%      >> set_interval_multiplier(30000)
%      >> set_animation_cycle('repeat')
%      >> disable_can_drop_frames()
%      >> set_animation_state('play_forward')
%      >> enable_can_drop_frames()
lasterr('This command must be executed from Opticks.')

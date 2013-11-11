% Test simple parsing as well as various control flow structures in MATLAB.
% This script does not test any Opticks functionality.

% fprintf('comments are not being ignored!')
fprintf('Running syntax_test. . .\n');
fprintf('   This script tests various control flow structures in MATLAB.\n')
x = 1; y = 2; z = 3;
if x ~= 1
   fprintf('   x did not assign correctly.\n')
end
if y ~= 2
   fprintf('   y did not assign correctly.\n')
end
if z ~= 3
   fprintf('   z did not assign correctly.\n')
end

if x ~= 1
   fprintf('   x did not assign correctly.\n')
elseif y ~= 2
   fprintf('   y did not assign correctly.\n')
elseif z ~= 3
   fprintf('   z did not assign correctly.\n')
else
   ;
end

if x + y + z == z + y + x
   ;
else
   fprintf('   error with addition inside an if statement.\n')
end

for x = 1:10
   if x^2 ~= x * x
      fprintf('   error with for loop squaring for %d.\n', x)
   end
end

fprintf('Finished running syntax_test.')

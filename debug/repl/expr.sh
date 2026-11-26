## tests common arithmetic expressions.

if ! ./cth repl '1+2;'; then
  echo "error in 'debug/repl/expr.sh' case 1"
  exit 1
elif ! ./cth repl '1+(2+3+(4+5))+6;'; then
  echo "error in 'debug/repl/expr.sh' case 2"
  exit 1
elif ! ./cth repl 'if(true) 1+2 else 3+4;'; then
  echo "error in 'debug/repl/expr.sh' case 3"
  exit 1
elif ! ./cth repl '(if(false) 1+2 else 3+4) :: i32;'; then
  echo "error in 'debug/repl/expr.sh' case 3"
  exit 1
elif ! ./cth repl '(true? 1+2 : 3+4) :: i32;'; then
  echo "error in 'debug/repl/expr.sh' case 5"
  exit 1
elif ! ./cth repl '(false? 1+2 : 3+4) :: i32;'; then
  echo "error in 'debug/repl/expr.sh' case 6"
  exit 1
elif ! ./cth repl '{1} + 2;'; then
  echo "error in 'debug/repl/expr.sh' case 7"
  exit 1
elif ! ./cth repl '{1+2} + 3;'; then
  echo "error in 'debug/repl/expr.sh' case 8"
  exit 1
elif ! ./cth repl '1 + {2};'; then
  echo "error in 'debug/repl/expr.sh' case 9"
  exit 1
elif ! ./cth repl '1 + {2} + 3;'; then
  echo "error in 'debug/repl/expr.sh' case 10"
  exit 1
elif ! ./cth repl '1 + {2 + 3} + 4;'; then
  echo "error in 'debug/repl/expr.sh' case 11"
  exit 1
elif ! ./cth repl '1 + {i32 x = 100; x + 10} + 4;'; then
  echo "error in 'debug/repl/expr.sh' case 11"
  exit 1
fi


exit 0
## tests common arithmetic expressions.

if ! ./cth repl '1+2;'; then
  echo "error in 'debug/repl/expr.sh' case 1"
  exit 1
elif ! ./cth repl '1+(2+3+(4+5))+6;'; then
  echo "error in 'debug/repl/expr.sh' case 2"
  exit 1
fi

exit 0
param order;
param n;
param givens{1..n, 1..n}, integer, >= 0, <= n, default 0;
/* the "givens" */
 
var x{i in 1..n, j in 1..n, k in 1..n}, binary;
/* x[i,j,k] = 1 means cell [i,j] is assigned number k */
 
s.t. fa{i in 1..n, j in 1..n, k in 1..n: givens[i,j] != 0}:
     x[i,j,k] = (if givens[i,j] = k then 1 else 0);
/* assign pre-defined numbers using the "givens" */
 
s.t. fb{i in 1..n, j in 1..n}: sum{k in 1..n} x[i,j,k] = 1;
/* each cell must be assigned exactly one number */
 
s.t. fc{i in 1..n, k in 1..n}: sum{j in 1..n} x[i,j,k] = 1;
/* cells in the same row must be assigned distinct numbers */
 
s.t. fd{j in 1..n, k in 1..n}: sum{i in 1..n} x[i,j,k] = 1;
/* cells in the same column must be assigned distinct numbers */
 
s.t. fe{I in 1..n by order, J in 1..n by order, k in 1..n}:
     sum{i in I..I+order-1, j in J..J+order-1} x[i,j,k] = 1;
/* cells in the same region must be assigned distinct numbers */
 
/* there is no need for an objective function here */
 
solve;
 
for {i in 1..n}
{
   for {j in 1..n}
   {
      printf " %d", sum{k in 1..n} x[i,j,k] * k;
      for {0..0: j = n} printf("\n");
   }
}

end;

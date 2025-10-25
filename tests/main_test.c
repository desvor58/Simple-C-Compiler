

int main()
{
    int loc_var = 6;
    int var2 = loc_var;
}


// some_fun(5, 3) * 5 + 3
//
//+
// 3
// *
//  5
//  fun_call
//   some_fun
//   5
//   3

// arr[index](5, 4) + 5
//
// fun_call_op
//   expr
//     get_by_index_op
//       ident arr
//       ident index
//   int lit 5
//   int lit 4

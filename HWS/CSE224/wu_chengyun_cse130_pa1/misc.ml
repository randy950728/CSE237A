(* CSE 130: Programming Assignment 1
 * misc.ml
 *)

(* sumList : int list -> int
	***** PUT DOCUMENTATION COMMENTS HERE *****
   This function takes in a list and returns an integer which
   represents the sum of the lsit. When a given list is empty
   the return value will be 0, else we simply add the first
   element of a list to the recursive call of sumList on the
   rest of list
*)

let rec sumList l =
	match l with
	|	[] 		-> 0
	| 	(h::t) 	-> h+sumList t;;

(* digitsOfInt : int -> int list
   ***** PUT DOCUMENTATION COMMENTS HERE *****
   Thist function maps a given positive integer to a list consists of all
   the digit within the given integer. The function is achieved by using
   tail recurssion, where we defined a helper function within the main
   function body. The helper function takes in 2 argument x and l, and
   on the way to the bottom of recurssion, the function push the least
   significant digit in x onto l using mod operator while also dividing
   x by 10 for the next layer of recursion.
   (see the digits function below for an example of what is expected)
 *)
let rec digitsOfInt n =
	let rec digit_help x =
		fun l ->
		if x<=0 then l
		else digit_help (x/10) ((x mod 10)::l)

	in digit_help n [] ;;

(* digits : int -> int list
 * (digits n) is the list of digits of n in the order in which they appear
 * in n
 * e.g. (digits 31243) is [3,1,2,4,3]
 *      (digits (-23422) is [2,3,4,2,2]
 *)

let digits n = digitsOfInt (abs n)


(* From http://mathworld.wolfram.com/AdditivePersistence.html
 * Consider the process of taking a number, adding its digits,
 * then adding the digits of the number derived from it, etc.,
 * until the remaining number has only one digit.
 * The number of additions required to obtain a single digit from a number n
 * is called the additive persistence of n, and the digit obtained is called
 * the digital root of n.
 * For example, the sequence obtained from the starting number 9876 is (9876, 30, 3), so
 * 9876 has an additive persistence of 2 and a digital root of 3.
 *)


(* ***** PROVIDE COMMENT BLOCKS FOR THE FOLLOWING FUNCTIONS ***** *)
(*
					additive Persistence
	The function is implemented by checking the current value
	of the given integer, if n has only 1 digit, the additive
	persistence is simply 0. Else, if the current digit has more
	than 1 digit, we calculate the additive sum of n and then
	call upon additive persistence function recursively on the
	value with just computed. Finally, we then return 1+the
	result of recursion.
*)
let rec additivePersistence n =
	if (n/10)>0 then
		let x = sumList (digitsOfInt n) in
		1+ (additivePersistence x)
	else
		0;;

(*
						digitalRoot
	The function is implemented by checking the current value
	of the given integer, if n has only 1 digit, the additive
	root is simply itself. Else, if the current digit has more
	than 1 digit, we calculate the additive sum of n and then
	call upon digitalRoot recurrsively on the value we computed
*)
let rec digitalRoot n =
	if (n/10)>0 then
		let l = digitsOfInt n in
		digitalRoot (sumList l)
	else
		n;;


(*
						listReverse
	The function is implemented by using tail recursion.To do
	so, we declare a helper function rev_help which takes in
	2 list, and as we grdually decent into each leayer of
	recurrsion, we move each element of the list from one onto
	the other. Once we have finished everything, we simply return
	the new list
*)
let rec listReverse l =
	let rec rev_help la =
		fun lb ->
		match la with
		| [] -> lb
		| (h::t) -> rev_help t (h::lb)

	in rev_help l [] ;;

(* explode : string -> char list
 * (explode s) is the list of characters in the string s in the order in
 *   which they appear
 * e.g.  (explode "Hello") is ['H';'e';'l';'l';'o']
 *)
let explode s =
  let rec _exp i =
    if i >= String.length s then [] else (s.[i])::(_exp (i+1)) in
  _exp 0


(*
						palindrome
	First we simply use explode to split a given string into a list
	of characters. Next, we simply use equal operator on the returned
	list and the reversed list
*)
let palindrome w =
	let l = explode w in
	l = listReverse l;;

(************** Add Testing Code Here ***************)

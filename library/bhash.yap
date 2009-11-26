/* 

This code implements hash-arrays.
It requires the hash key to be a ground term.

It relies on dynamic array code.

*/
:- source.
:- yap_flag(unknown,error).
:- style_check(all).

:- module(b_hash, [   b_hash_new/1,
		      b_hash_new/2,
		      b_hash_new/4,
		      b_hash_lookup/3,
		      b_hash_update/3,
		      b_hash_update/4,
		      b_hash_insert_new/4,
		      b_hash_insert/4
		    ]).

:- use_module(library(terms), [ term_hash/4 ]).


:- meta_predicate(b_hash_new(-,+,3,2)).

array_default_size(2048).

b_hash_new(hash(Keys, Vals, Size, N, _, _)) :-
	array_default_size(Size),
	array(Keys, Size),
	array(Vals, Size),
	create_mutable(0, N).

b_hash_new(hash(Keys, Vals, Size, N, _, _), Size) :-
	array(Keys, Size),
	array(Vals, Size),
	create_mutable(0, N).

b_hash_new(hash(Keys,Vals, Size, N, HashF, CmpF), Size, HashF, CmpF) :-
	array(Keys, Size),
	array(Vals, Size),
	create_mutable(0, N).

b_hash_lookup(Key, Val, hash(Keys, Vals, Size, _, F, CmpF)):-
	hash_f(Key, Size, Index, F),
	fetch_key(Keys, Index, Size, Key, CmpF, ActualIndex),
	array_element(Vals, ActualIndex, Mutable),
	get_mutable(Val, Mutable).

fetch_key(Keys, Index, Size, Key, CmpF, ActualIndex) :-
	array_element(Keys, Index, El),
	nonvar(El),
	(
	    cmp_f(CmpF, El, Key)
	->
	    Index = ActualIndex
	;
	    I1 is (Index+1) mod Size,
	    fetch_key(Keys, I1, Size, Key, CmpF, ActualIndex)
	).

b_hash_update(Hash, Key, NewVal):-
	Hash = hash(Keys, Vals, Size, _, F, CmpF),
	hash_f(Key,Size,Index,F),
	fetch_key(Keys, Index, Size, Key, CmpF, ActualIndex),
	array_element(Vals, ActualIndex, Mutable),
	update_mutable(NewVal, Mutable).

b_hash_update(Hash, Key, OldVal, NewVal):-
	Hash = hash(Keys, Vals, Size, _, F, CmpF),
	hash_f(Key,Size,Index,F),
	fetch_key(Keys, Index, Size, Key, CmpF, ActualIndex),
	array_element(Vals, ActualIndex, Mutable),
	get_mutable(OldVal, Mutable),
	update_mutable(NewVal, Mutable).

b_hash_insert(Hash, Key, NewVal, NewHash):-
	Hash = hash(Keys, Vals, Size, N, F, CmpF),
	hash_f(Key,Size,Index,F),
	find_or_insert(Keys, Index, Size, N, CmpF, Vals, Key, NewVal, Hash, NewHash).

find_or_insert(Keys, Index, Size, N, CmpF, Vals, Key, NewVal, Hash, NewHash) :-
	array_element(Keys, Index, El),
	(
	    var(El)
	->
	    add_element(Keys, Index, Size, N, Vals, Key, NewVal, Hash, NewHash)
	;
	    cmp_f(CmpF, El, Key)
	->
	     % do rb_update
	    array_element(Vals, Index, Mutable),
	    update_mutable(NewVal, Mutable)
        ;
	    I1 is (Index+1) mod Size,
	    find_or_insert(Keys, I1, Size, N, CmpF, Vals, Key, NewVal, Hash, NewHash)
	).

b_hash_insert_new(Hash, Key, NewVal, NewHash):-
	Hash = hash(Keys, Vals, Size, N, F, CmpF),
	hash_f(Key,Size,Index,F),
	find_or_insert_new(Keys, Index, Size, N, CmpF, Vals, Key, NewVal, Hash, NewHash).

find_or_insert_new(Keys, Index, Size, N, CmpF, Vals, Key, NewVal, Hash, NewHash) :-
	array_element(Keys, Index, El),
	(
	    var(El)
	->
	    add_element(Keys, Index, Size, N, Vals, Key, NewVal, Hash, NewHash)
	;
	    cmp_f(CmpF, El, Key)
	->
	    fail
        ;
	    I1 is (Index+1) mod Size,
	    find_or_insert_new(Keys, I1, Size, N, CmpF, Vals, Key, NewVal, Hash, NewHash)
	).

add_element(Keys, Index, Size, N, Vals, Key, NewVal, Hash, NewHash) :-
	get_mutable(NEls, N),
	NN is NEls+1,
	update_mutable(NN, N),
	(
	    NN > Size/3
	->
	    expand_array(Hash, NewHash)
	;
	    Hash = NewHash
	),
	array_element(Keys, Index, Key),
	update_mutable(NN, N),
	array_element(Vals, Index, Mutable),
	create_mutable(NewVal, Mutable).

expand_array(Hash, hash(NewKeys, NewVals, NewSize, X, F, CmpF)) :-
	Hash = hash(Keys, Vals, Size, X, F, CmpF),
	new_size(Size, NewSize),
	array(NewKeys, NewSize),
	array(NewVals, NewSize),
	copy_hash_table(Size, Keys, Vals, F, NewSize, NewKeys, NewVals).

new_size(Size, NewSize) :-
	Size > 1048576, !,
	NewSize is Size+1048576.
new_size(Size, NewSize) :-
	NewSize is Size*2.

copy_hash_table(0, _, _, _, _, _, _) :- !.
copy_hash_table(I1, Keys, Vals, F, Size, NewKeys, NewVals) :-
	I is I1-1,
	array_element(Keys, I, Key),
	nonvar(Key), !,
	array_element(Vals, I, Val),
	insert_el(Key, Val, Size, F, NewKeys, NewVals),
	copy_hash_table(I, Keys, Vals, F, Size, NewKeys, NewVals).
copy_hash_table(I1, Keys, Vals, F, Size, NewKeys, NewVals) :-
	I is I1-1,
	copy_hash_table(I, Keys, Vals, F, Size, NewKeys, NewVals).

insert_el(Key, Val, Size, F, NewKeys, NewVals) :-
	hash_f(Key,Size,Index, F),
	find_free(Index, Size, NewKeys, TrueIndex),
	array_element(NewKeys, TrueIndex, Key),
	array_element(NewVals, TrueIndex, Val).

find_free(Index, Size, Keys, NewIndex) :-
	array_element(Keys, Index, El),
	(
	    var(El)
	->
	    NewIndex = Index
	;
	    I1 is (Index+1) mod Size,
	    find_free(I1, Size, Keys, NewIndex)
	).

hash_f(Key, Size, Index, F) :-
	var(F), !,
	term_hash(Key,-1,Size,Index).
hash_f(Key, Size, Index, F) :-
	call(F, Key, Size, Index).

cmp_f(F, A, B) :-
	var(F), !,
	A == B.
cmp_f(F, A, B) :-
	call(F, A, B).


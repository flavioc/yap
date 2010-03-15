/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		corout.pl						 *
* Last rev:								 *
* mods:									 *
* comments:	Coroutines implementation				 *
*									 *
*************************************************************************/


:- module('$coroutining',[
			  op(1150, fx, block)
				%dif/2,
				%when/2,
				%block/1,
				%wait/1,
				%frozen/2
			 ]).


attr_unify_hook(DelayList, _) :-
	wake_delays(DelayList).
	
wake_delays([]).
wake_delays(Delay.List) :-
	wake_delay(Delay),
	wake_delays(List).

%
% Interface to attributed variables.
%
wake_delay(redo_dif(Done, X, Y)) :-
	redo_dif(Done, X, Y).
wake_delay(redo_freeze(Done, V, Goal)) :-
	redo_freeze(Done, V, Goal).
wake_delay(redo_eq(Done, X, Y, Goal)) :-
	redo_eq(Done, X, Y, Goal, G).
wake_delay(redo_ground(Done, X, Goal)) :-
	redo_ground(Done, X, Goal).

attribute_goals(Var) -->
	{ get_attr(Var, '$coroutining', Delays) },
	attgoal_for_delays(Delays, Var).
	
attgoal_for_delays([], V) --> [].
attgoal_for_delays([G|AllAtts], V) -->
	attgoal_for_delay(G, V),
	attgoal_for_delays(AllAtts, V).
	
attgoal_for_delay(redo_dif(Done, X, Y), V) --> { var(Done), first_att(dif(X,Y), V) }, !, [prolog:dif(X,Y)].
attgoal_for_delay(redo_freeze(Done, V, Goal), V) --> { var(Done) },  !, [prolog:freeze(V,Goal)].
attgoal_for_delay(redo_eq(Done, X, Y, Goal), V) --> { var(Done), first_att(Goal, V) }, !, [prolog:when(X=Y,Goal)].
attgoal_for_delay(redo_ground(Done, X, Goal), V) --> { var(Done) },  !, [prolog:when(ground(X),Goal)].
attgoal_for_delay(_, V) --> [].

				%
% operators defined in this module:
%
prolog:freeze(V, G) :-
	var(V), !,
	freeze_goal(V,G).
prolog:freeze(_, G) :-
	'$execute'(G).

freeze_goal(V,VG) :-
	var(VG), !,
	'$current_module'(M),
	internal_freeze(V, redo_freeze(_Done,V,M:VG)).
freeze_goal(V,M:G) :- !,
	internal_freeze(V, redo_freeze(_Done,V,M:G)).
freeze_goal(V,G) :-
	'$current_module'(M),
	internal_freeze(V, redo_freeze(_Done,V,M:G)).

%
%
% Dif is tricky because we need to wake up on the two variables being
% bound together, or on any variable of the term being bound to
% another. Also, the day YAP fully supports infinite rational trees,
% dif should work for them too. Hence, term comparison should not be
% implemented in Prolog.
%
% This is the way dif works. The '$can_unify' predicate does not know
% anything about dif semantics, it just compares two terms for
% equaility and is based on compare. If it succeeds without generating
% a list of variables, the terms are equal and dif fails. If it fails,
% dif succeeds.
%
% If it succeeds but it creates a list of variables, dif creates
% suspension records for all these variables on the '$redo_dif'(V,
% X, Y) goal. V is a flag that says whether dif has completed or not,
% X and Y are the original goals. Whenever one of these variables is
% bound, it calls '$redo_dif' again. '$redo_dif' will then check whether V
% was bound. If it was, dif has succeeded and redo_dif just
% exits. Otherwise, '$redo_dif' will call dif again to see what happened.
%
% Dif needs two extensions from the suspension engine:
%
% First, it needs
% for the engine to be careful when binding two suspended
% variables. Basically, in this case the engine must be sure to wake
% up one of the goals, as they may make dif fail. The way the engine
% does so is by searching the list of suspended variables, and search
% whether they share a common suspended goal. If they do, that
% suspended goal is added to the WokenList.
%
% Second, thanks to dif we may try to suspend on the same variable
% several times. dif calls a special version of freeze that checks
% whether that is in fact the case.
%
prolog:dif(X, Y) :-
	'$can_unify'(X, Y, LVars), !,
	LVars = [_|_], 
	dif_suspend_on_lvars(LVars, redo_dif(_Done, X, Y)).
prolog:dif(_, _).


dif_suspend_on_lvars([], _).
dif_suspend_on_lvars([H|T], G) :-
	internal_freeze(H, G),
	dif_suspend_on_lvars(T, G).

%
% This predicate is called whenever a variable dif was suspended on is
% bound. Note that dif may have already executed successfully.
%
% Three possible cases: dif has executed and Done is bound; we redo
% dif and the two terms either unify, hence we fail, or may unify, and
% we try to increase the number of suspensions; last, the two terms
% did not unify, we are done, so we succeed and bind the Done variable.
%
redo_dif(Done, _, _) :- nonvar(Done), !.
redo_dif(Done, X, Y) :-
	'$can_unify'(X, Y, LVars), !,
	LVars = [_|_],
	dif_suspend_on_lvars(LVars, redo_dif(Done, X, Y)).
redo_dif('$done', _, _).

% If you called nonvar as condition for when, then you may find yourself
% here.
%
% someone else (that is Cond had ;) did the work, do nothing
%
redo_freeze(Done, _, _) :- nonvar(Done), !.
%
% We still have some more conditions: continue the analysis.
%
redo_freeze(Done, _, '$when'(C, G, Done)) :- !,
	'$when'(C, G, Done).
	
%
% check if the variable was really bound
%
redo_freeze(Done, V, G) :- var(V), !,
	internal_freeze(V, redo_freeze(Done,V,G)).
%
% I can't believe it: we're done and can actually execute our
% goal. Notice we have to say we are done, otherwise someone else in
% the disjunction might decide to wake up the goal themselves.
%
redo_freeze('$done', _, G) :-
	'$execute'(G).

%
% eq is a combination of dif and freeze
redo_eq(Done, _, _, _, _) :- nonvar(Done), !.
redo_eq(_, X, Y, _, G) :-
	'$can_unify'(X, Y, LVars),
	LVars = [_|_], !,
	dif_suspend_on_lvars(LVars, G).
redo_eq(Done, _, _, when(C, G, Done), _) :- !,
	when(C, G, Done).
redo_eq('$done', _ ,_ , Goal, _) :-
	'$execute'(Goal).

%
% ground is similar to freeze
redo_ground(Done, _, _) :- nonvar(Done), !.
redo_ground(Done, X, Goal) :-
	'$non_ground'(X, Var), !,
	internal_freeze(Var, redo_ground(Done, X, Goal)).
redo_ground(Done, _, when(C, G, Done)) :- !,
	when(C, G, Done).
redo_ground('$done', _, Goal) :-
	'$execute'(Goal).


%
% support for when/2 built-in
%
prolog:when(Conds,Goal) :-
	'$current_module'(Mod),
	prepare_goal_for_when(Goal, Mod, ModG),
	when(Conds, ModG, Done, [], LG), !,
%write(vsc:freezing(LG,Done)),nl,
	suspend_when_goals(LG, Done).
prolog:when(_,Goal) :-
	'$execute'(Goal).

%
% support for when/2 like declaration.
%
%
% when will block on a conjunction or disjunction of nonvar, ground,
% ?=, where ?= is both terms being bound together
%
%
'$declare_when'(Cond, G) :-
	generate_code_for_when(Cond, G, Code),
	'$current_module'(Module),
	'$$compile'(Code, Code, 5, Module), fail.
'$declare_when'(_,_).

%
% use a meta interpreter for now
%
generate_code_for_when(Conds, G,
	( G :- when(Conds, ModG, Done, [], LG), !,
	suspend_when_goals(LG, Done)) ) :-
	'$current_module'(Mod),
	prepare_goal_for_when(G, Mod, ModG).


%
% make sure we have module info for G!
%
prepare_goal_for_when(G, Mod, Mod:call(G)) :- var(G), !.
prepare_goal_for_when(M:G, _,  M:G) :- !.
prepare_goal_for_when(G, Mod, Mod:G).
	

%
% now for the important bit
%

% Done is used to synchronise: when it is bound someone else did the
% goal and we can give up.
%
% $when/5 and $when_suspend succeds when there is need to suspend a goal
%
%
when(V, G, Done, LG0, LGF) :- var(V), !,
	'$do_error'(instantiation_error,when(V,G)).
when(nonvar(V), G, Done, LG0, LGF) :-
	when_suspend(nonvar(V), G, Done, LG0, LGF).
when(?=(X,Y), G, Done, LG0, LGF) :-
	when_suspend(?=(X,Y), G, Done, LG0, LGF).
when(ground(T), G, Done, LG0, LGF) :-
	when_suspend(ground(T), G, Done, LG0, LGF).
when((C1, C2), G, Done, LG0, LGF) :-
	% leave it open to continue with when.
	(
	    when(C1, when(C2, G, Done), Done, LG0, LGI)
        ->
	    LGI = LGF
        ;
	    % we solved C1, great, now we just have to solve C2!
	    when(C2, G, Done, LG0, LGF)
        ).
when((G1 ; G2), G, Done, LG0, LGF) :-
	when(G1, G, Done, LG0, LGI),
	when(G2, G, Done, LGI, LGF).

%
% Auxiliary predicate called from within a conjunction.
% Repeat basic code for when,  as inserted in first clause for predicate.
%
when(_, _, Done) :-
	nonvar(Done), !.
when(Cond, G, Done) :-
	when(Cond, G, Done, [], LG),
	!,
	suspend_when_goals(LG, Done).
when(_, G, '$done') :-
	'$execute'(G).

%
% Do something depending on the condition!
%
% some one else did the work.
%
when_suspend(_, _, Done, _, []) :- nonvar(Done), !.
%
% now for the serious stuff.
%
when_suspend(nonvar(V), G, Done, LG0, LGF) :-
	try_freeze(V, G, Done, LG0, LGF).
when_suspend(?=(X,Y), G, Done, LG0, LGF) :-
	try_eq(X, Y, G, Done, LG0, LGF).
when_suspend(ground(X), G, Done, LG0, LGF) :-
	try_ground(X, G, Done, LG0, LGF).


try_freeze(V, G, Done, LG0, LGF) :-
	var(V),
	LGF = ['$coroutining':internal_freeze(V, redo_freeze(Done, V, G))|LG0].

try_eq(X, Y, G, Done, LG0, LGF) :- 
	'$can_unify'(X, Y, LVars), LVars = [_|_],
	LGF = ['$coroutining':dif_suspend_on_lvars(LVars, redo_eq(Done, X, Y, G))|LG0].

try_ground(X, G, Done, LG0, LGF) :-
	'$non_ground'(X, Var),    % the C predicate that succeds if
				  % finding out the term is nonground
				  % and gives the first variable it
				  % finds. Notice that this predicate
				  % must know about svars.
	LGF = ['$coroutining':internal_freeze(Var, redo_ground(Done, X, G))| LG0].

%
% When executing a when, if nobody succeeded, we need to create suspensions.
%
suspend_when_goals([], _).
suspend_when_goals(['$coroutining':internal_freeze(V, G)|Ls], Done) :-
	var(Done), !,
	internal_freeze(V, G),
	suspend_when_goals(Ls, Done).
suspend_when_goals([dif_suspend_on_lvars(LVars, G)|LG], Done) :-
	var(Done), !,
	dif_suspend_on_lvars(LVars, G),
	suspend_when_goals(LG, Done).
suspend_when_goals([_|_], _).

%
% Support for wait declarations on goals.
% Or we also use the more powerful, SICStus like, "block" declarations.
%
% block or wait declarations must precede the first clause.
%

%
% I am using the simplest solution now: I'll add an extra clause at
% the beginning of the procedure to do this work. This creates a
% choicepoint and make things a bit slower, but it's probably not as
% significant as the remaining overheads.
%
prolog:'$block'(Conds) :-
	generate_blocking_code(Conds, _, Code),
	'$current_module'(Module),
	'$$compile'(Code, Code, 5, Module), fail.
prolog:'$block'(_).

generate_blocking_code(Conds, G, Code) :-
	'$extract_head_for_block'(Conds, G),
	'$recorded'('$blocking_code','$code'(G,OldConds),R), !,
	erase(R),
	functor(G, Na, Ar),
	'$current_module'(M),
	abolish(M:Na, Ar),
	generate_blocking_code((Conds,OldConds), G, Code).	
generate_blocking_code(Conds, G, (G :- (If, !, when(When, G)))) :-
	extract_head_for_block(Conds, G),
	recorda('$blocking_code','$code'(G,Conds),_),
	generate_body_for_block(Conds, G, If, When).

%
% find out what we are blocking on.
%
extract_head_for_block((C1, _), G) :- !,
	extract_head_for_block(C1, G).
extract_head_for_block(C, G) :-
	functor(C, Na, Ar),
	functor(G, Na, Ar).

%
% If we suspend on the conditions, we should continue
% execution. If we don't suspend we should fail so that we can take
% the next clause. To
% know what we have to do we just test how many variables we suspended
% on ;-).
%

%
% We generate code as follows:
%
% block a(-,-,?) 
%
% (var(A1), var(A2) -> true ; fail), !, when((nonvar(A1);nonvar(A2)),G).
%
% block a(-,-,?), a(?,-, -)
%
% (var(A1), var(A2) -> true ; (var(A2), var(A3) -> true ; fail)), !,
%                   when(((nonvar(A1);nonvar(A2)),(nonvar(A2);nonvar(A3))),G).

generate_body_for_block((C1, C2), G, (Code1 -> true ; Code2), (WhenConds,OtherWhenConds)) :- !,
	generate_for_cond_in_block(C1, G, Code1, WhenConds),
	generate_body_for_block(C2, G, Code2, OtherWhenConds).
generate_body_for_block(C, G, (Code -> true ; fail), WhenConds) :-
	generate_for_cond_in_block(C, G, Code, WhenConds).

generate_for_cond_in_block(C, G, Code, Whens) :-
	C =.. [_|Args],
	G =.. [_|GArgs],
	fetch_out_variables_for_block(Args,GArgs,L0Vars),
	add_blocking_vars(L0Vars, LVars),
	generate_for_each_arg_in_block(LVars, Code, Whens).

add_blocking_vars([], [_]) :- !.
add_blocking_vars(LV, LV).

fetch_out_variables_for_block([], [], []).
fetch_out_variables_for_block(['?'|Args], [_|GArgs], LV) :-
	fetch_out_variables_for_block(Args, GArgs, LV).
fetch_out_variables_for_block(['-'|Args], [GArg|GArgs],
	       [GArg|LV]) :-
	fetch_out_variables_for_block(Args, GArgs, LV).

generate_for_each_arg_in_block([], false, true).
generate_for_each_arg_in_block([V], var(V), nonvar(V)) :- !.
generate_for_each_arg_in_block([V|L], (var(V),If), (nonvar(V);Whens)) :-
	generate_for_each_arg_in_block(L, If, Whens).


%
% The wait declaration is a simpler and more efficient version of block.
%
prolog:'$wait'(Na/Ar) :-
	functor(S, Na, Ar),
	arg(1, S, A),
	'$current_module'(M),
	'$$compile'((S :- var(A), !, freeze(A, S)), (S :- var(A), !, freeze(A, S)), 5, M), fail.
prolog:'$wait'(_).

frozen(V, G) :- nonvar(V), !,
	'$do_error'(type_error(variable,V),frozen(V,G)).
frozen(V, LG) :-
	'$attributes':get_conj_from_attvars([V], LG).

%internal_freeze(V,G) :-
%	attributes:get_att(V, 0, Gs), write(G+Gs),nl,fail.
internal_freeze(V,G) :-
	update_att(V, G).

update_att(V, G) :-
	attributes:get_module_atts(V, '$coroutining'(_,Gs)),
	not_vmember(G, Gs), !,
	attributes:put_module_atts(V, '$coroutining'(_,[G|Gs])).
update_att(V, G) :-
	attributes:put_module_atts(V, '$coroutining'(_,[G])).
	  

not_vmember(_, []).
not_vmember(V, [V1|DonesSoFar]) :-
	V \== V1,
	not_vmember(V, DonesSoFar).

first_att(T, V) :-
	term_variables(T, Vs),
	check_first_attvar(Vs, V).

check_first_attvar(V.Vs, V0) :- attvar(V), !, V == V0.
check_first_attvar(_.Vs, V0) :-
	check_first_attvar(Vs, V0).


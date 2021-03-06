/*  $Id$

    Part of SWI-Prolog RDF parser

    Author:  Jan Wielemaker
    E-mail:  jan@swi.psy.uva.nl
    WWW:     http://www.swi.psy.uva.nl/projects/SWI-Prolog/
    Copying: LGPL-2.  See the file COPYING or http://www.gnu.org

    Copyright (C) 1990-2000 SWI, University of Amsterdam. All rights reserved.
*/

:- module(rdf_triple,
	  [ rdf_triples/2,		% +Parsed, -Tripples
	    rdf_triples/3,		% +Parsed, -Tripples, +Tail
	    rdf_reset_ids/0,		% Reset gensym id's
	    rdf_start_file/2,		% +Options, -Cleanup
	    rdf_end_file/1,		% +Cleanup
	    anon_prefix/1		% Prefix for anonynmous resources
	  ]).
:- use_module(library(gensym)).
:- use_module(rdf_parser).

/** <module> Create triples from intermediate representation

Convert the output of xml_to_rdf/3  from   library(rdf)  into  a list of
triples of the format described   below. The intermediate representation
should be regarded a proprietary representation.

	rdf(Subject, Predicate, Object).

Where `Subject' is

	* Atom
	The subject is a resource
	
	* each(URI)
	URI is the URI of an RDF Bag
	
	* prefix(Pattern)
	Pattern is the prefix of a fully qualified Subject URI

And `Predicate' is

	* Atom
	The predicate is always a resource

And `Object' is

	* Atom
	URI of Object resource

	* literal(Value)
	Literal value (Either a single atom or parsed XML data)
*/

%%	rdf_triples(+Term, -Triples) is det.
%%	rdf_triples(+Term, -Tridpples, +Tail) is det.
%
%	Convert an object as parsed by rdf.pl into a list of rdf/3
%	triples.  The identifier of the main object created is returned
%	by rdf_triples/3.
%
%	Input is the `content' of the RDF element in the format as
%	generated by load_structure(File, Term, [dialect(xmlns)]).
%	rdf_triples/3 can process both individual descriptions as
%	well as the entire content-list of an RDF element.  The first
%	mode is suitable when using library(sgml) in `call-back' mode.

rdf_triples(RDF, Tripples) :-
	rdf_triples(RDF, Tripples, []).

rdf_triples([]) --> !,
	[].
rdf_triples([H|T]) --> !,
	rdf_triples(H),
	rdf_triples(T).
rdf_triples(Term) -->
	triples(Term, _).

%%	triples(-Triples, -Id, +In, -Tail)
%
%	DGC set processing the output of xml_to_rdf/3.  In Id, the identifier
%	of the main description or container is returned.

triples(container(Type, Id, Elements), Id) --> !,
	{ container_id(Type, Id)
	},
	rdf(Id, rdf:type, rdf:Type),
	container(Elements, 1, Id).
triples(description(Type, About, BagId, Props), Subject) -->
	{ var(About),
	  var(BagId),
	  share_blank_nodes(true)
	}, !,
	(   { shared_description(description(Type, Props), Subject)
	    }
	->  []
	;   { make_id('__Description', Id)
	    },
	    triples(description(Type, about(Id), BagId, Props), Subject),
	    { assert_shared_description(description(Type, Props), Subject)
	    }
	).
triples(description(description, IdAbout, BagId, Props), Subject) --> !,
	{ description_id(IdAbout, Subject)
	},
	properties(Props, BagId, Subject).
triples(description(Type, IdAbout, BagId, Props), Subject) -->
	{ description_id(IdAbout, Subject),
	  name_to_type_uri(Type, TypeURI)
	},
	properties([ rdf:type = TypeURI
		   | Props
		   ], BagId, Subject).
triples(unparsed(Data), Id) -->
	{ make_id('__Error', Id),
	  print_message(error, rdf(unparsed(Data)))
	},
	[].


name_to_type_uri(NS:Local, URI) :- !,
	atom_concat(NS, Local, URI).
name_to_type_uri(URI, URI).

		 /*******************************
		 *	    CONTAINERS		*
		 *******************************/

container([], _, _) -->
	[].
container([H0|T0], N, Id) -->
	li(H0, N, Id),
	{ NN is N + 1
	},
	container(T0, NN, Id).

li(li(Nid, V), _, Id) --> !,
	rdf(Id, rdf:Nid, V).
li(V, N, Id) -->
	triples(V, VId), !,
	{ atom_concat('_', N, Nid)
	},
	rdf(Id, rdf:Nid, VId).
li(V, N, Id) -->
	{ atom_concat('_', N, Nid)
	},
	rdf(Id, rdf:Nid, V).
	
container_id(_, Id) :-
	nonvar(Id), !.
container_id(Type, Id) :-
	container_base(Type, Base),
	make_id(Base, Id).

container_base('Bag', '__Bag').
container_base('Seq', '__Seq').
container_base('Alt', '__Alt').


		 /*******************************
		 *	    DESCRIPTIONS	*
		 *******************************/

:- thread_local
	node_id/2,			% nodeID --> ID
	unique_id/1.			% known rdf:ID

rdf_reset_node_ids :-
	retractall(node_id(_,_)),
	retractall(unique_id(_)).

description_id(Id, Id) :-
	var(Id), !,
	make_id('__Description', Id).
description_id(about(Id), Id).
description_id(id(Id), Id) :-
	(   unique_id(Id)
	->  print_message(error, rdf(redefined_id(Id)))
	;   assert(unique_id(Id))
	).
description_id(each(Id), each(Id)).
description_id(prefix(Id), prefix(Id)).
description_id(node(NodeID), Id) :-
	(   node_id(NodeID, Id)
	->  true
	;   make_id('__Node', Id),
	    assert(node_id(NodeID, Id))
	).

properties(PlRDF, BagId, Subject) -->
	{ nonvar(BagId)
	}, !,
	rdf(BagId, rdf:type, rdf:'Bag'),
	properties(PlRDF, 1, Statements, [], Subject),
	fill_bag(Statements, 1, BagId).
properties(PlRDF, _BagId, Subject) -->
	properties(PlRDF, 1, [], [], Subject).


fill_bag([], _, _) -->
	[].
fill_bag([H|T], N, BagId) -->
	{ NN is N + 1,
	  atom_concat('_', N, ElemId)
	},
	rdf(BagId, rdf:ElemId, H),
	fill_bag(T, NN, BagId).


properties([], _, Bag, Bag, _) -->
	[].
properties([H0|T0], N, Bag0, Bag, Subject) -->
	property(H0, N, NN, Bag0, Bag1, Subject),
	properties(T0, NN, Bag1, Bag, Subject).

%%	property(Property, N, NN, Subject)// is det.
%	
%	Generate triples for {Subject,  Pred,   Object}.  Also generates
%	triples for Object if necessary.
%	
%	@param Property	One of
%	
%		* Pred = Object
%		Used for normal statements
%		* id(Id, Pred = Object)
%		Used for reified statements

property(Pred0 = Object, N, NN, BagH, BagT, Subject) --> % inlined object
	triples(Object, Id), !,
	{ li_pred(Pred0, Pred, N, NN)
	},
	statement(Subject, Pred, Id, _, BagH, BagT).
property(Pred0 = collection(Elems), N, NN, BagH, BagT, Subject) --> !,
	{ li_pred(Pred0, Pred, N, NN)
	},
	statement(Subject, Pred, Object, _Id, BagH, BagT),
	collection(Elems, Object).
property(Pred0 = Object, N, NN, BagH, BagT, Subject) --> !,
	{ li_pred(Pred0, Pred, N, NN)
	},
	statement(Subject, Pred, Object, _Id, BagH, BagT).
property(id(Id, Pred0 = Object), N, NN, BagH, BagT, Subject) -->
	triples(Object, ObjectId), !,
	{ li_pred(Pred0, Pred, N, NN)
	},
	statement(Subject, Pred, ObjectId, Id, BagH, BagT).
property(id(Id, Pred0 = collection(Elems)), N, NN, BagH, BagT, Subject) --> !,
	{ li_pred(Pred0, Pred, N, NN)
	},
	statement(Subject, Pred, Object, Id, BagH, BagT),
	collection(Elems, Object).
property(id(Id, Pred0 = Object), N, NN, BagH, BagT, Subject) -->
	{ li_pred(Pred0, Pred, N, NN)
	},
	statement(Subject, Pred, Object, Id, BagH, BagT).

%%	statement(+Subject, +Pred, +Object, +Id, +BagH, -BagT)
%	
%	Add a statement to the model. If nonvar(Id), we reinify the
%	statement using the given Id.

statement(Subject, Pred, Object, Id, BagH, BagT) -->
	rdf(Subject, Pred, Object),
	{   BagH = [Id|BagT]
	->  statement_id(Id)
	;   BagT = BagH
	},
	(   { nonvar(Id)
	    }
	->  rdf(Id, rdf:type, rdf:'Statement'),
	    rdf(Id, rdf:subject, Subject),
	    rdf(Id, rdf:predicate, Pred),
	    rdf(Id, rdf:object, Object)
	;   []
	).


statement_id(Id) :-
	nonvar(Id), !.
statement_id(Id) :-
	make_id('__Statement', Id).

%%	li_pred(+Pred, -Pred, +Nth, -NextNth)
%	
%	Transform rdf:li predicates into _1, _2, etc.

li_pred(rdf:li, rdf:Pred, N, NN) :- !,
	NN is N + 1,
	atom_concat('_', N, Pred).
li_pred(Pred, Pred, N, N).
	
%%	collection(+Elems, -Id)
%	
%	Handle the elements of a collection and return the identifier
%	for the whole collection in Id.

collection([], Nil) -->
	{ global_ref(rdf:nil, Nil)
	}.
collection([H|T], Id) -->
	triples(H, HId),
	{ make_id('__List', Id)
	},
	rdf(Id, rdf:type, rdf:'List'),
	rdf(Id, rdf:first, HId),
	rdf(Id, rdf:rest, TId),
	collection(T, TId).


rdf(S0, P0, O0) -->
	{ global_ref(S0, S),
	  global_ref(P0, P),
	  global_obj(O0, O)
	},
	[ rdf(S, P, O) ].


global_ref(URI, URI) :-
	var(URI), !.
global_ref(rdf:Local, Global) :-
	rdf_name_space(NS), !,
	atom_concat(NS, Local, Global).
global_ref(NS:Local, Global) :- !,
	atom_concat(NS, Local, Global).
global_ref(URI, URI).

global_obj(V, V) :-
	var(V), !.
global_obj(literal(type(Local, X)), literal(type(Global, X))) :- !,
	global_ref(Local, Global).
global_obj(literal(X), literal(X)) :- !.
global_obj(Local, Global) :-
	global_ref(Local, Global).


		 /*******************************
		 *	       SHARING		*
		 *******************************/

:- thread_local
	shared_description/3,		% +Hash, +Term, -Subject
	share_blank_nodes/1,		% Boolean
	shared_nodes/1.			% counter

reset_shared_descriptions :-
	retractall(shared_description(_,_,_)),
	retractall(shared_nodes(_)).

shared_description(Term, Subject) :-
	term_hash(Term, Hash),
	shared_description(Hash, Term, Subject),
	(   retract(shared_nodes(N))
	->  N1 is N + 1
	;   N1 = 1
	),
	assert(shared_nodes(N1)).
	    

assert_shared_description(Term, Subject) :-
	term_hash(Term, Hash),
	assert(shared_description(Hash, Term, Subject)).


		 /*******************************
		 *	      START/END		*
		 *******************************/

%%	rdf_start_file(+Options, -Cleanup) is det.
%
%	Initialise for the translation of a file.

rdf_start_file(Options, Cleanup) :-
	rdf_reset_node_ids,		% play safe
	reset_shared_descriptions,
	set_bnode_sharing(Options, C1),
	set_anon_prefix(Options, C2),
	add_cleanup(C1, C2, Cleanup).

%%	rdf_end_file(:Cleanup) is det.
%	
%	Cleanup reaching the end of an RDF file.

rdf_end_file(Cleanup) :-
	rdf_reset_node_ids,
	(   shared_nodes(N)
	->  print_message(informational, rdf(shared_blank_nodes(N)))
	;   true
	),
	reset_shared_descriptions,
	Cleanup.

set_bnode_sharing(Options, erase(Ref)) :-
	option(blank_nodes(Share), Options, noshare),
	(   Share == share
	->  assert(share_blank_nodes(true), Ref), !
	;   Share == noshare
	->  fail			% next clause
	;   throw(error(domain_error(share, Share), _))
	).
set_bnode_sharing(_, true).

set_anon_prefix(Options, erase(Ref)) :-
	option(base_uri(BaseURI), Options, []),
	BaseURI \== [], !,
	concat_atom(['__', BaseURI, '#'], AnonBase),
	asserta(anon_prefix(AnonBase), Ref).
set_anon_prefix(_, true).

add_cleanup(true, X, X) :- !.
add_cleanup(X, true, X) :- !.
add_cleanup(X, Y, (X, Y)).


		 /*******************************
		 *	       UTIL		*
		 *******************************/

%%	anon_prefix(-Prefix) is semidet.
%
%	If defined, it is the prefix used to generate a blank node.

:- thread_local
	anon_prefix/1.

make_id(For, ID) :-
	anon_prefix(Prefix), !,
	atom_concat(Prefix, For, Base),
	gensym(Base, ID).
make_id(For, ID) :-
	gensym(For, ID).

anon_base('__Bag').
anon_base('__Seq').
anon_base('__Alt').
anon_base('__Description').
anon_base('__Statement').
anon_base('__List').
anon_base('__Node').

%%	rdf_reset_ids is det.
%
%	Utility predicate to reset the gensym counters for the various
%	generated identifiers.  This simplifies debugging and matching
%	output with the stored desired output (see rdf_test.pl).

rdf_reset_ids :-
	anon_prefix(Prefix), !,
	(   anon_base(Base),
	    atom_concat(Prefix, Base, X),
	    reset_gensym(X),
	    fail
	;   true
	).
rdf_reset_ids :-
	(   anon_base(Base),
	    reset_gensym(Base),
	    fail
	;   true
	).

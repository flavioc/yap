/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz - University of Porto, Portugal
  File:        tab.sub.i
  version:
  comment:     subsumption + retroactive tabling related instructions   
                                                                     
**********************************************************************/
  
  PBOp(table_load_cons_answer, Otapl)
#ifdef TABLING_CALL_SUBSUMPTION
    CELL *ans_tmplt;
    ans_node_ptr ans_node;
    continuation_ptr next;
    
    ans_tmplt = (CELL *) (LOAD_CP(B) + 1);
    
    next = continuation_next(LOAD_CP(B)->cp_last_answer);
    ans_node = continuation_answer(next);
    
    if(continuation_has_next(next)) {
      restore_loader_node(next);
    } else {
      pop_loader_node();
    }
    
    consume_answer_leaf(ans_node, ans_tmplt, CONSUME_SUBSUMPTIVE_ANSWER);
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (table_load_cons_answer)");
#endif
  ENDPBOp();
  
  PBOp(table_run_completed, Otapl)

#ifdef TABLING_RETROACTIVE
    run_completed:

    INIT_PREFETCH()

    sg_fr_ptr cons_sg_fr = CONS_CP(B)->cp_sg_fr;

    if(cons_sg_fr == (sg_fr_ptr)538475492)
      {
        printf("AHHHHHH\n");
        exit(1);
      }
#ifdef FDEBUG
    dprintf("===> TABLE_RUN_COMPLETED ");
    printSubgoalTriePath(stdout, cons_sg_fr);
    dprintf("\n");
#endif

    switch(SgFr_type(cons_sg_fr)) {
      case RETROACTIVE_CONSUMER_SFT: {
        retroactive_fr_ptr sg_fr = (retroactive_fr_ptr)cons_sg_fr;
        dep_fr_ptr dep_fr = CONS_CP(B)->cp_dep_fr;
        continuation_ptr cont;

        if(SgFr_state(SgFr_producer(sg_fr)) < complete) {
          dprintf("producer not completed!\n");

          if(!SgFr_is_local_consumer(sg_fr)) {
            /* transform into consumer */
            if(dep_fr) {
              B->cp_ap = ANSWER_RESOLUTION;
            } else {
              add_dependency_frame(sg_fr, B);
            }
            goto answer_resolution;
          }

          build_next_retroactive_consumer_return_list(sg_fr);

          if(SgFr_try_answer(sg_fr))
            cont = continuation_next(SgFr_try_answer(sg_fr));
          else
            cont = SgFr_first_answer(sg_fr);

          if(cont) {
            dprintf("Consuming...\n");
            /* as long we can consume answers we
             * can avoid being a real consumer */
            consume_next_retroactive_answer(cont, sg_fr);
          } else if(SgFr_saved_cp(SgFr_producer(sg_fr))) {
            retroactive_fr_ptr prod = SgFr_producer(sg_fr);
            choiceptr cp = SgFr_saved_cp(prod);

            rebind_variables(cp->cp_tr, B->cp_tr);
            B = cp;
            SgFr_saved_cp(prod) = NULL;
            H = HBREG = PROTECT_FROZEN_H(B);
            restore_yaam_reg_cpdepth(B);
            TR = TR_FZ;
            ENV = B->cp_env;
            PREG = (yamop *) B->cp_ap;
            PREFETCH_OP(PREG);
            YENV = ENV;
            dprintf("Going to next!\n");
            GONext();
          }

          /* no more answers to consume, transform this node
             into a consumer */
          dprintf("Not completed!\n");
          add_dependency_frame(sg_fr, B);
          B = B->cp_b;
          if(B->cp_ap == NULL)
            B->cp_ap = COMPLETION;
          goto fail;
        }

        /* locate lost consumers of this subgoal */
        dep_fr_ptr top = LOCAL_top_dep_fr;

        while(top && YOUNGER_CP(DepFr_cons_cp(top), B)) {
          choiceptr cp = DepFr_cons_cp(top);

          if(cp->cp_ap == RUN_COMPLETED)
          {
            /* launch consumer */
            dprintf("Lost consumer found!\n");

            restore_bindings(B->cp_tr, cp->cp_tr);
            DepFr_backchain_cp(top) = B;

            B = cp;
            TR = TR_FZ;
            if (TR != B->cp_tr)
              TRAIL_LINK(B->cp_tr);

            H = HBREG = PROTECT_FROZEN_H(B);
            restore_yaam_reg_cpdepth(B);
            CPREG = B->cp_cp;
            ENV = B->cp_env;
            RESTORE_TOP_GEN_SG(top);
            PREG = RUN_COMPLETED;
            PREFETCH_OP(PREG);
            YENV = ENV;
            GONext();
          }

          top = DepFr_next(top);
        }

        if(SgFr_state(sg_fr) < complete) {
          dprintf("just completed!\n");
          /* producer subgoal just completed */
          build_next_retroactive_consumer_return_list(sg_fr);
          mark_retroactive_consumer_as_completed(sg_fr);
        }

        if(dep_fr) {
          dprintf("ABOLISH DEP_FR\n");

          cont = continuation_next(DepFr_last_answer(dep_fr));
          REMOVE_DEP_FR_FROM_STACK(dep_fr);
          SgFr_num_deps((retroactive_fr_ptr)sg_fr) = 0;
          FREE_DEPENDENCY_FRAME(dep_fr);

          if(!cont) {
            TABLING_close_alt(B);
            B = B->cp_b;
            SET_BB(PROTECT_FROZEN_B(B));
            goto fail;
          }
        } else if(SgFr_try_answer(sg_fr)) {
          /* some answers were consumed before */
          cont = continuation_next(SgFr_try_answer(sg_fr));

          if(!cont) {
            /* fail sooner */
            TABLING_close_alt(B);
            B = B->cp_b;
            SET_BB(PROTECT_FROZEN_B(B));
            goto fail;
          }
        } else {
          /* first answer to consume */
          if(SgFr_has_no_answers(sg_fr)) {
            /* goto fail */
            B = B->cp_b;
            SET_BB(PROTECT_FROZEN_B(B));
            goto fail;
          }

          cont = SgFr_first_answer(sg_fr);
        }

        CELL *answer_template;

        /* ... and store a loader! */
        if(continuation_has_next(cont)) {
          restore_general_node();
          transform_node_into_loader(B, cons_sg_fr, cont, LOAD_CONS_ANSWER);
          answer_template = LOADER_ANSWER_TEMPLATE(B);
        } else {
          answer_template = GENERATOR_ANSWER_TEMPLATE(B, sg_fr);
          pop_general_node();
        }

        /* load first answer */
        ans_node_ptr ans_node = continuation_answer(cont);

        consume_answer_leaf(ans_node, answer_template,
            CONSUME_SUBSUMPTIVE_ANSWER);
      }
      break;
      case SUBSUMED_CONSUMER_SFT: {
        dprintf("RUN_COMPLETED SUBSUMED CONSUMER!\n");
        subcons_fr_ptr sg_fr = (subcons_fr_ptr)cons_sg_fr;
        dep_fr_ptr dep_fr = CONS_CP(B)->cp_dep_fr;
        subprod_fr_ptr prod_sg = SgFr_producer(sg_fr);

        if(SgFr_state(sg_fr) == changed) {
          sg_node_ptr leaf = SgFr_leaf(sg_fr);

          dprintf("CONSUMER SUBGOAL HAS CHANGED\n");
          SgFr_num_deps(sg_fr)--;
          if(SgFr_num_deps(sg_fr) == 0) {
            dprintf("DELETING CONSUMER AT LAST\n");
            free_consumer_subgoal_data(sg_fr);
            FREE_SUBCONS_SUBGOAL_FRAME(sg_fr);
          }

          /* set to new */
          sg_fr_ptr new_sg = (sg_fr_ptr)TrNode_sg_fr(leaf);

          CONS_CP(B)->cp_sg_fr = new_sg;
          DepFr_sg_fr(dep_fr) = new_sg;
          DepFr_last_answer(dep_fr) = (continuation_ptr)CONSUMER_DEFAULT_LAST_ANSWER(new_sg, dep_fr);
          transform_consumer_answer_template(new_sg, B);
          goto run_completed;
        }

        if(SgFr_state(prod_sg) == dead) {
          dprintf("DEAD\n");
          REMOVE_DEP_FR_FROM_STACK(dep_fr);

          /* decrement reference counting and delete it */
          SgFr_num_deps((sg_fr_ptr)prod_sg)--;
          if(SgFr_num_deps((sg_fr_ptr)prod_sg) == 0)
            abolish_incomplete_subsumptive_producer_subgoal((sg_fr_ptr)prod_sg);

          /* create a new producer subgoal frame */
          new_subsumptive_producer_subgoal_frame(prod_sg, SgFr_code(sg_fr), SgFr_leaf(sg_fr));
          SgFr_choice_point(prod_sg) = B;
          reinsert_subgoal_frame((sg_fr_ptr)prod_sg, B);

          /* delete the dependency frame and the old consumer subgoal frame */
          SgFr_num_deps(sg_fr)--;
          FREE_DEPENDENCY_FRAME(dep_fr);
          if(SgFr_num_deps(sg_fr) == 0) {
            free_consumer_subgoal_data(sg_fr);
            FREE_SUBCONS_SUBGOAL_FRAME(sg_fr);
          } else {
            SgFr_state(sg_fr) = changed;
          }

          cons_sg_fr = (sg_fr_ptr)prod_sg;
          GEN_CP(B)->cp_sg_fr = cons_sg_fr;
          GEN_CP(B)->cp_dep_fr = NULL;
          transform_consumer_answer_template(cons_sg_fr, B);
          restart_code_execution(cons_sg_fr);
        } else if(SgFr_state(prod_sg) < complete) {
          B->cp_ap = ANSWER_RESOLUTION;
          goto answer_resolution;
        }

        dprintf("just completed sub!\n");

        /* producer subgoal just completed */
        REMOVE_DEP_FR_FROM_STACK(dep_fr);
        complete_dependency_frame(dep_fr);

        build_next_subsumptive_consumer_return_list(sg_fr);

        continuation_ptr cont = continuation_next(DepFr_last_answer(dep_fr));

        if(!cont) {
          /* fail sooner */
          B = B->cp_b;
          SET_BB(PROTECT_FROZEN_B(B));
          goto fail;
        }

        CELL *answer_template;

        /* transform into a loader! */
        if(continuation_has_next(cont)) {
          dprintf("transform into sub loader\n");
          restore_general_node();
          transform_node_into_loader(B, cons_sg_fr, cont, LOAD_CONS_ANSWER);
          answer_template = LOADER_ANSWER_TEMPLATE(B);
        } else {
          answer_template = GENERATOR_ANSWER_TEMPLATE(B, sg_fr);
          pop_general_node();
        }

        /* load answer */
        consume_answer_leaf(continuation_answer(cont), answer_template,
            CONSUME_SUBSUMPTIVE_ANSWER);
      }
      break;
      case SUBSUMPTIVE_PRODUCER_SFT:
      case VARIANT_PRODUCER_SFT: {
        dprintf("RUN_COMPLETED VARIANT CONSUMER!\n");
        sg_fr_ptr sg_fr = cons_sg_fr;
        dep_fr_ptr dep_fr = CONS_CP(B)->cp_dep_fr;
        int is_sub_transform = DepFr_is_subtransform(dep_fr);
        DepFr_clear_subtransform(dep_fr);
        DepFr_clear_restarter(dep_fr);

        if(is_sub_transform) {
          dprintf("transform answer_template\n");
          transform_consumer_answer_template(sg_fr, B);
        }

        if(SgFr_state(sg_fr) == suspended) {
          /* restart subgoal */
          REMOVE_DEP_FR_FROM_STACK(dep_fr);
          SgFr_state(sg_fr) = evaluating;
          GEN_CP(B)->cp_dep_fr = NULL; /* local_dep */
          SET_TOP_GEN_SG(sg_fr);
          FREE_DEPENDENCY_FRAME(dep_fr);
          SgFr_choice_point(sg_fr) = B;

          if(is_sub_transform) {
            dprintf("Is subtransform!\n");
#ifdef FDEBUG
            printSubgoalTriePath(stdout, sg_fr);
            dprintf("\n");
#endif
            reinsert_subgoal_frame(sg_fr, B);
            restart_code_execution(sg_fr);
          } else {
            B->cp_ap = TRY_ANSWER;

            reinsert_subgoal_frame(sg_fr, B);
            SgFr_try_answer(sg_fr) = DepFr_last_answer(dep_fr);
            dprintf("Going to try_answer_jump\n");
            goto try_answer_jump;
          }
        } else if(SgFr_state(sg_fr) < complete) {
          B->cp_ap = ANSWER_RESOLUTION;
          goto answer_resolution;
        }

        dprintf("just completed variant node!\n");

        /* producer subgoal just completed */
        REMOVE_DEP_FR_FROM_STACK(dep_fr);
        complete_dependency_frame(dep_fr);

        continuation_ptr cont = continuation_next(DepFr_last_answer(dep_fr));

        if(!cont) {
          /* fail sooner */
          B = B->cp_b;
          SET_BB(PROTECT_FROZEN_B(B));
          goto fail;
        }

        CELL *answer_template;

        /* transform into a loader! */
        if(continuation_has_next(cont)) {
          dprintf("transform into var loader\n");
          restore_general_node();
          transform_node_into_loader(B, sg_fr, cont, LOAD_ANSWER);
          answer_template = LOADER_ANSWER_TEMPLATE(B);
        } else {
          answer_template = GENERATOR_ANSWER_TEMPLATE(B, sg_fr);
          pop_general_node();
        }

        /* load answer */
        consume_answer_leaf(continuation_answer(cont), answer_template,
            CONSUME_VARIANT_ANSWER);
      }
      break;
      default:
      dprintf("default fail!\n");
      exit(1);
      break;
    }

    printf("CANT BE HERE!!!\n");
    exit(1);
    END_PREFETCH()
#endif /* TABLING_RETROACTIVE */
  ENDPBOp();
  
  PBOp(table_try_retroactive_answer, Otapl)
#ifdef TABLING_RETROACTIVE
    retroactive_fr_ptr sg_fr;
    ans_node_ptr ans_node = NULL;
    continuation_ptr next_cont;
    
    dprintf("===> TABLE_TRY_RETROACTIVE_ANSWER\n");
    
    sg_fr = (retroactive_fr_ptr)GEN_CP(B)->cp_sg_fr;
    next_cont = continuation_next(SgFr_try_answer(sg_fr));
    
    if(next_cont) {
      CELL *answer_template = (CELL *)(GEN_CP(B) + 1) + SgFr_arity(sg_fr);
      
      SgFr_try_answer(sg_fr) = next_cont;
#ifdef TABLING_RETROACTIVE
      Bind_and_Trail(&SgFr_executing(sg_fr), (Term)B);
#endif /* TABLING_RETROACTIVE */

      ans_node = continuation_answer(next_cont);
      TrNode_set_ans(ans_node);
      H = HBREG = PROTECT_FROZEN_H(B);
      restore_yaam_reg_cpdepth(B);
      CPREG = B->cp_cp;
      ENV = B->cp_env;
      
      PREG = (yamop *) CPREG;
      PREFETCH_OP(PREG);
      CONSUME_RETROACTIVE_ANSWER(ans_node, answer_template, sg_fr);
      YENV = ENV;
      GONext();
    } else {
      yamop *code_ap;
      PREG = SgFr_code(sg_fr);
      if (PREG->opc == Yap_opcode(_table_try)) {
	      /* table_try */
	      code_ap = NEXTOP(PREG,Otapl);
	      PREG = PREG->u.Otapl.d;
      } else if (PREG->opc == Yap_opcode(_table_try_single)) {
	      /* table_try_single */
	      code_ap = COMPLETION;
	      PREG = PREG->u.Otapl.d;
      } else {
	      /* table_try_me */
	      code_ap = PREG->u.Otapl.d;
	      PREG = NEXTOP(PREG,Otapl);
      }
      PREFETCH_OP(PREG);
      restore_generator_node(SgFr_arity(sg_fr), code_ap);
      YENV = (CELL *) PROTECT_FROZEN_B(B);
      set_cut(YENV, B->cp_b);
      SET_BB(NORM_CP(YENV));
      allocate_environment();
      GONext();
    }
#else
    PREG = PREG->u.Otapl.d;
    PREFETCH_OP(PREG);
    GONext();
#endif /* TABLING_RETROACTIVE */
  ENDPBOp();
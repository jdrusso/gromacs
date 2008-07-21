#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef _domdec_h
#define _domdec_h

#include "typedefs.h"
#include "vsite.h"

#ifdef GMX_MPI
#include <mpi.h>
#endif

extern int ddglatnr(gmx_domdec_t *dd,int i);
/* Returns the global topology atom number belonging to local atom index i.
 * This function is intended for writing ascii output
 * and returns atom numbers starting at 1.
 * When dd=NULL returns i+1.
 */

extern bool dd_filled_nsgrid_home(gmx_domdec_t *dd);
/* Is the ns grid already filled with the home particles? */

extern void dd_store_state(gmx_domdec_t *dd,t_state *state);
/* Store the global cg indices of the home cgs in state,
 * so it can be reset, even after a new DD partitioning.
 */

extern void dd_get_ns_ranges(gmx_domdec_t *dd,int icg,
			     int *jcg0,int *jcg1,ivec shift0,ivec shift1);

extern int dd_natoms_vsite(gmx_domdec_t *dd);

extern void dd_get_constraint_range(gmx_domdec_t *dd,
				    int *at_start,int *at_end);

extern real dd_cutoff(gmx_domdec_t *dd);

extern real dd_cutoff_mbody(gmx_domdec_t *dd);

extern bool gmx_pmeonlynode(t_commrec *cr,int nodeid);
/* Return if nodeid in cr->mpi_comm_mysim is a PME-only node */

extern void get_pme_ddnodes(t_commrec *cr,int pmenodeid,
			    int *nmy_ddnodes,int **my_ddnodes,int *node_peer);
/* Returns the set of DD nodes that communicate with pme node cr->nodeid */

extern void dd_set_tric_dir(gmx_domdec_t *dd,matrix box);
/* Set the triclinic box information in dd */

extern int dd_pme_maxshift(gmx_domdec_t *dd);
/* Returns the maximum shift for coordinate communication in PME */

extern void make_dd_communicators(FILE *fplog,t_commrec *cr,int dd_node_order);

extern gmx_domdec_t *init_domain_decomposition(FILE *fplog,
					       t_commrec *cr,ivec nc,
					       real comm_distance_min,
					       real rconstr,
					       bool bDynLoadBal,real dlb_scale,
					       char *sizex,
					       char *sizey,
					       char *sizez,
					       t_topology *top,matrix box,
					       t_inputrec *ir);

extern void set_dd_parameters(FILE *fplog,gmx_domdec_t *dd,real dlb_scale,
			      t_topology *top,t_inputrec *ir,t_forcerec *fr,
			      matrix box);

extern void setup_dd_grid(FILE *fplog,gmx_domdec_t *dd);

extern void dd_collect_vec(gmx_domdec_t *dd,t_block *cgs_gl,
			   t_state *state_local,rvec *lv,rvec *v);

extern void dd_collect_state(gmx_domdec_t *dd,t_block *cgs_gl,
			     t_state *state_local,t_state *state);

enum { ddCyclStep, ddCyclPPduringPME, ddCyclF, ddCyclPME, ddCyclNr };

extern void dd_cycles_add(gmx_domdec_t *dd,float cycles,int ddCycl);
/* Add the wallcycle count to the DD counter */

extern void dd_force_flop_start(gmx_domdec_t *dd,t_nrnb *nrnb);
/* Start the force flop count */

extern void dd_force_flop_stop(gmx_domdec_t *dd,t_nrnb *nrnb);
/* Stop the force flop count */

extern void dd_move_x(gmx_domdec_t *dd,matrix box,rvec x[],rvec buf[]);
/* buf should should have size natoms (of the whole system)
 * although in most cases far less will be used.
 */

extern void dd_move_f(gmx_domdec_t *dd,rvec f[],rvec buf[],rvec *fshift);
/* buf should should have size natoms (of the whole system)
 * although in most cases far less will be used.
 * When fshift!=NULL the shift forces are updated to obtain
 * the correct virial from the single sum including f.
 */

extern void dd_partition_system(FILE            *fplog,
				int             step,
				t_commrec       *cr,
                                bool            bMasterState,
				t_state         *state_global,
				t_topology      *top_global,
				t_inputrec      *ir,
				t_state         *state_local,
				rvec            **f,
				rvec            **buf,
				t_mdatoms       *mdatoms,
				t_topology      *top_local,
				t_forcerec      *fr,
				gmx_vsite_t     *vsite,
				gmx_shellfc_t   shellfc,
				gmx_constr_t    constr,
				t_nrnb          *nrnb,
				gmx_wallcycle_t wcycle,
				bool            bVerbose);
/* Partition the system over the nodes.
 * step is only used for printing error messages.
 * If bMasterState==TRUE then state_global from the master node is used,
 * else state_local is redistributed between the nodes.
 */

void print_dd_statistics(t_commrec *cr,t_inputrec *ir,FILE *fplog);

/* In domdec_con.c */

extern void dd_move_f_vsites(gmx_domdec_t *dd,rvec *f,rvec *fshift);

extern void dd_clear_f_vsites(gmx_domdec_t *dd,rvec *f);

extern void dd_move_x_constraints(gmx_domdec_t *dd,matrix box,
				  rvec *x0,rvec *x1);
/* Move x0 and also x1 if x1!=NULL */

extern void dd_move_x_vsites(gmx_domdec_t *dd,matrix box,rvec *x);

extern void dd_clear_local_constraint_indices(gmx_domdec_t *dd);

extern void dd_clear_local_vsite_indices(gmx_domdec_t *dd);

extern int dd_make_local_vsites(gmx_domdec_t *dd,int at_start,t_ilist *lil);

extern int dd_make_local_constraints(gmx_domdec_t *dd,int at_start,t_iatom *ia,
				     gmx_constr_t constr,int nrec);

extern void init_domdec_constraints(gmx_domdec_t *dd,
				    int natoms,t_topology *top,
				    gmx_constr_t constr);

extern void init_domdec_vsites(gmx_domdec_t *dd,int natoms);


/* In domdec_top.c */

extern void dd_print_missing_interactions(FILE *fplog,t_commrec *cr,
					  int local_count);

extern void dd_make_reverse_top(FILE *fplog,
				gmx_domdec_t *dd,t_topology *top,
				gmx_vsite_t *vsite,gmx_constr_t constr,
				t_inputrec *ir,bool bBCheck);

extern void dd_make_local_cgs(gmx_domdec_t *dd,t_block *lcgs);

extern void dd_make_local_top(FILE *fplog,gmx_domdec_t *dd,
			      matrix box,real rc,rvec cellsize_min,ivec npulse,
			      t_forcerec *fr,gmx_vsite_t *vsite,
			      t_topology *top,t_topology *ltop);

extern t_topology *dd_init_local_top(t_topology *top_global);

extern void dd_init_local_state(gmx_domdec_t *dd,
				t_state *state_global,t_state *local_state);

/* In domdec_setup.h */

extern void dd_check_pme_grid(FILE *fplog,bool bStdErr,int npme,
			      t_inputrec *ir);

extern void dd_choose_grid(FILE *fplog,
			   t_commrec *cr,gmx_domdec_t *dd,t_inputrec *ir,
			   t_topology *top,matrix box,real dlb_scale,
			   real cellsize_limit,real cutoff_mbody,
			   bool bInterCGBondeds,bool bInterCGMultiBody);

#endif	/* _domdec_h */



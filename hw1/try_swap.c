static int
try_swap(float t,
	 float *cost,
	 float *bb_cost,
	 float *timing_cost,
	 float rlim,
	 int place_cost_type,
	 float **old_region_occ_x,
	 float **old_region_occ_y,
	 int num_regions,
	 boolean fixed_pins,
	 enum e_place_algorithm place_algorithm,
	 float timing_tradeoff,
	 float inverse_prev_bb_cost,
	 float inverse_prev_timing_cost,
	 float *delay_cost,
	 int *x_lookup)
{

    /* Picks some block and moves it to another spot.  If this spot is   *
     * occupied, switch the blocks.  Assess the change in cost function  *
     * and accept or reject the move.  If rejected, return 0.  If        *
     * accepted return 1.  Pass back the new value of the cost function. * 
     * rlim is the range limiter.                                                                            */

    int b_from, x_to, y_to, z_to, x_from, y_from, z_from, b_to;
    int i, k, inet, keep_switch, num_of_pins, max_pins_per_fb;
    int num_nets_affected, bb_index;
    float delta_c, bb_delta_c, timing_delta_c, delay_delta_c, newcost;
    static struct s_bb *bb_coord_new = NULL;
    static struct s_bb *bb_edge_new = NULL;
    static int *nets_to_update = NULL, *net_block_moved = NULL;

    max_pins_per_fb = 0;
    for(i = 0; i < num_types; i++)
	{
	    max_pins_per_fb =
		max(max_pins_per_fb, type_descriptors[i].num_pins);
	}

    /* Allocate the local bb_coordinate storage, etc. only once. */

    if(bb_coord_new == NULL)
	{
	    bb_coord_new = (struct s_bb *)my_malloc(2 * max_pins_per_fb *
						    sizeof(struct s_bb));
	    bb_edge_new = (struct s_bb *)my_malloc(2 * max_pins_per_fb *
						   sizeof(struct s_bb));
	    nets_to_update =
		(int *)my_malloc(2 * max_pins_per_fb * sizeof(int));
	    net_block_moved =
		(int *)my_malloc(2 * max_pins_per_fb * sizeof(int));
	}


    delay_delta_c = 0.0;
    b_from = my_irand(num_blocks - 1);

    /* If the pins are fixed we never move them from their initial    *
     * random locations.  The code below could be made more efficient *
     * by using the fact that pins appear first in the block list,    *
     * but this shouldn't cause any significant slowdown and won't be *
     * broken if I ever change the parser so that the pins aren't     *
     * necessarily at the start of the block list.                    */

    if(fixed_pins == TRUE)
	{
	    while(block[b_from].type == IO_TYPE)
		{
		    b_from = my_irand(num_blocks - 1);
		}
	}

    x_from = block[b_from].x;
    y_from = block[b_from].y;
    z_from = block[b_from].z;

    if(!find_to
       (x_from, y_from, block[b_from].type, rlim, x_lookup, &x_to, &y_to))
	return FALSE;

    /* Make the switch in order to make computing the new bounding *
     * box simpler.  If the cost increase is too high, switch them *
     * back.  (block data structures switched, clbs not switched   *
     * until success of move is determined.)                       */

    z_to = 0;
    if(grid[x_to][y_to].type->capacity > 1)
	{
	    z_to = my_irand(grid[x_to][y_to].type->capacity - 1);
	}
    if(grid[x_to][y_to].blocks[z_to] == EMPTY)
	{			/* Moving to an empty location */
	    b_to = EMPTY;
	    block[b_from].x = x_to;
	    block[b_from].y = y_to;
	    block[b_from].z = z_to;
	}
    else
	{			/* Swapping two blocks */
	    b_to = grid[x_to][y_to].blocks[z_to];
	    block[b_to].x = x_from;
	    block[b_to].y = y_from;
	    block[b_to].z = z_from;

	    block[b_from].x = x_to;
	    block[b_from].y = y_to;
	    block[b_from].z = z_to;
	}

    /* Now update the cost function.  May have to do major optimizations *
     * here later.                                                       */

    /* I'm using negative values of temp_net_cost as a flag, so DO NOT   *
     * use cost functions that can go negative.                          */

    delta_c = 0;		/* Change in cost due to this swap. */
    bb_delta_c = 0;
    timing_delta_c = 0;

    num_of_pins = block[b_from].type->num_pins;

    num_nets_affected = find_affected_nets(nets_to_update, net_block_moved,
					   b_from, b_to, num_of_pins);

    if(place_cost_type == NONLINEAR_CONG)
	{
	    save_region_occ(old_region_occ_x, old_region_occ_y, num_regions);
	}

    bb_index = 0;		/* Index of new bounding box. */

    for(k = 0; k < num_nets_affected; k++)
	{
	    inet = nets_to_update[k];

	    /* If we swapped two blocks connected to the same net, its bounding box *
	     * doesn't change.                                                      */

	    if(net_block_moved[k] == FROM_AND_TO)
		continue;

	    if(net[inet].num_sinks < SMALL_NET)
		{
		    get_non_updateable_bb(inet, &bb_coord_new[bb_index]);
		}
	    else
		{
		    if(net_block_moved[k] == FROM)
			update_bb(inet, &bb_coord_new[bb_index],
				  &bb_edge_new[bb_index], x_from, y_from,
				  x_to, y_to);
		    else
			update_bb(inet, &bb_coord_new[bb_index],
				  &bb_edge_new[bb_index], x_to, y_to, x_from,
				  y_from);
		}

	    if(place_cost_type != NONLINEAR_CONG)
		{
		    temp_net_cost[inet] =
			get_net_cost(inet, &bb_coord_new[bb_index]);
		    bb_delta_c += temp_net_cost[inet] - net_cost[inet];
		}
	    else
		{
		    /* Rip up, then replace with new bb. */
		    update_region_occ(inet, &bb_coords[inet], -1,
				      num_regions);
		    update_region_occ(inet, &bb_coord_new[bb_index], 1,
				      num_regions);
		}

	    bb_index++;
	}

    if(place_cost_type == NONLINEAR_CONG)
	{
	    newcost = nonlinear_cong_cost(num_regions);
	    bb_delta_c = newcost - *bb_cost;
	}


    if(place_algorithm == NET_TIMING_DRIVEN_PLACE ||
       place_algorithm == PATH_TIMING_DRIVEN_PLACE)
	{
	    /*in this case we redefine delta_c as a combination of timing and bb.  *
	     *additionally, we normalize all values, therefore delta_c is in       *
	     *relation to 1*/

	    comp_delta_td_cost(b_from, b_to, num_of_pins, &timing_delta_c,
			       &delay_delta_c);

	    delta_c =
		(1 - timing_tradeoff) * bb_delta_c * inverse_prev_bb_cost +
		timing_tradeoff * timing_delta_c * inverse_prev_timing_cost;
	}
    else
	{
	    delta_c = bb_delta_c;
	}


    keep_switch = assess_swap(delta_c, t);

    /* 1 -> move accepted, 0 -> rejected. */

    if(keep_switch)
	{
	    *cost = *cost + delta_c;
	    *bb_cost = *bb_cost + bb_delta_c;


	    if(place_algorithm == NET_TIMING_DRIVEN_PLACE ||
	       place_algorithm == PATH_TIMING_DRIVEN_PLACE)
		{
		    /*update the point_to_point_timing_cost and point_to_point_delay_cost 
		     * values from the temporary values */
		    *timing_cost = *timing_cost + timing_delta_c;
		    *delay_cost = *delay_cost + delay_delta_c;

		    update_td_cost(b_from, b_to, num_of_pins);
		}

	    /* update net cost functions and reset flags. */

	    bb_index = 0;

	    for(k = 0; k < num_nets_affected; k++)
		{
		    inet = nets_to_update[k];

		    /* If we swapped two blocks connected to the same net, its bounding box *
		     * doesn't change.                                                      */

		    if(net_block_moved[k] == FROM_AND_TO)
			{
			    temp_net_cost[inet] = -1;
			    continue;
			}

		    bb_coords[inet] = bb_coord_new[bb_index];
		    if(net[inet].num_sinks >= SMALL_NET)
			bb_num_on_edges[inet] = bb_edge_new[bb_index];

		    bb_index++;

		    net_cost[inet] = temp_net_cost[inet];
		    temp_net_cost[inet] = -1;
		}

	    /* Update fb data structures since we kept the move. */
	    /* Swap physical location */
	    grid[x_to][y_to].blocks[z_to] = b_from;
	    grid[x_from][y_from].blocks[z_from] = b_to;

	    if(EMPTY == b_to)
		{		/* Moved to an empty location */
		    grid[x_to][y_to].usage++;
		    grid[x_from][y_from].usage--;
		}
	}

    else
	{			/* Move was rejected.  */

	    /* Reset the net cost function flags first. */
	    for(k = 0; k < num_nets_affected; k++)
		{
		    inet = nets_to_update[k];
		    temp_net_cost[inet] = -1;
		}

	    /* Restore the block data structures to their state before the move. */
	    block[b_from].x = x_from;
	    block[b_from].y = y_from;
	    block[b_from].z = z_from;
	    if(b_to != EMPTY)
		{
		    block[b_to].x = x_to;
		    block[b_to].y = y_to;
		    block[b_to].z = z_to;
		}

	    /* Restore the region occupancies to their state before the move. */
	    if(place_cost_type == NONLINEAR_CONG)
		{
		    restore_region_occ(old_region_occ_x, old_region_occ_y,
				       num_regions);
		}
	}

    return (keep_switch);
}

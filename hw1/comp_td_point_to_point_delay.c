static float
comp_td_point_to_point_delay(int inet,
			     int ipin)
{

    /*returns the delay of one point to point connection */

    int source_block, sink_block;
    int delta_x, delta_y;
    t_type_ptr source_type, sink_type;
    float delay_source_to_sink;

    delay_source_to_sink = 0.;

    source_block = net[inet].node_block[0];
    source_type = block[source_block].type;

    sink_block = net[inet].node_block[ipin];
    sink_type = block[sink_block].type;

    assert(source_type != NULL);
    assert(sink_type != NULL);

    delta_x = abs(block[sink_block].x - block[source_block].x);
    delta_y = abs(block[sink_block].y - block[source_block].y);

    /* TODO low priority: Could be merged into one look-up table */
    /* Note: This heuristic is terrible on Quality of Results.  
     * A much better heuristic is to create a more comprehensive lookup table but
     * it's too late in the release cycle to do this.  Pushing until the next release */
    if(source_type == IO_TYPE)
	{
	    if(sink_type == IO_TYPE)
		delay_source_to_sink = delta_io_to_io[delta_x][delta_y];
	    else
		delay_source_to_sink = delta_io_to_fb[delta_x][delta_y];
	}
    else
	{
	    if(sink_type == IO_TYPE)
		delay_source_to_sink = delta_fb_to_io[delta_x][delta_y];
	    else
		delay_source_to_sink = delta_fb_to_fb[delta_x][delta_y];
	}
    if(delay_source_to_sink < 0)
	{
	    printf
		("Error in comp_td_point_to_point_delay in place.c, bad delay_source_to_sink value\n");
	    exit(1);
	}

    if(delay_source_to_sink < 0.)
	{
	    printf
		("Error in comp_td_point_to_point_delay in place.c, delay is less than 0\n");
	    exit(1);
	}

    return (delay_source_to_sink);
}


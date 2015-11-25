/* * Copyright (c) 2015 Regents of the University of California. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The names of its contributors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *********************************************************************************************** *
 * CARLsim
 * created by: (MDR) Micah Richert, (JN) Jayram M. Nageswaran
 * maintained by:
 * (MA) Mike Avery <averym@uci.edu>
 * (MB) Michael Beyeler <mbeyeler@uci.edu>,
 * (KDC) Kristofor Carlson <kdcarlso@uci.edu>
 * (TSC) Ting-Shuo Chou <tingshuc@uci.edu>
 *
 * CARLsim available from http://socsci.uci.edu/~jkrichma/CARLsim/
 * Ver 5/22/2015
 */

// include CARLsim user interface
#include <carlsim.h>

#define N_EXC 800
#define N_INH 200

int main() {
	// create a network on GPU
	int numGPUs = 2;
	int randSeed = 42;
	float pConn = 100.0f / (N_EXC + N_INH); // connection probability
	CARLsim sim("test kernel", GPU_MODE, USER, numGPUs, randSeed);

	// configure the network
	int gExc = sim.createGroup("exc", N_EXC, EXCITATORY_NEURON, 1);
	sim.setNeuronParameters(gExc, 0.02f, 0.2f, -65.0f, 8.0f); // RS

	int gInh = sim.createGroup("inh", N_INH, INHIBITORY_NEURON, 0);
	sim.setNeuronParameters(gInh, 0.1f, 0.2f, -65.0f, 2.0f); // FS
	//int gExc2 = sim.createGroup("exc", N_EXC, EXCITATORY_NEURON);
	//sim.setNeuronParameters(gExc2, 0.02f, 0.2f, -65.0f, 8.0f); // RS

	int gInput = sim.createSpikeGeneratorGroup("input", N_EXC, EXCITATORY_NEURON);

	sim.connect(gInput, gExc, "one-to-one", RangeWeight(30.0f), 1.0f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);
	sim.connect(gExc, gExc, "random", RangeWeight(6.0f), pConn, RangeDelay(1, 20), RadiusRF(-1), SYN_FIXED);
	sim.connect(gExc, gInh, "random", RangeWeight(6.0f), pConn, RangeDelay(1, 20), RadiusRF(-1), SYN_FIXED);
	sim.connect(gInh, gExc, "random", RangeWeight(5.0f), pConn * 1.25f, RangeDelay(1), RadiusRF(-1), SYN_FIXED);

	sim.setConductances(false);

	//sim.setESTDP(gExc, true, STANDARD, ExpCurve(0.1f/100, 20, -0.12f/100, 20));

	// build the network
	sim.setupNetwork();

	// set some monitors
	//SpikeMonitor* smInput = sim.setSpikeMonitor(gInput, "DEFAULT");
	SpikeMonitor* smExc = sim.setSpikeMonitor(gExc, "NULL");
	SpikeMonitor* smInh = sim.setSpikeMonitor(gInh, "NULL");
	SpikeMonitor* smInput = sim.setSpikeMonitor(gInput, "NULL");
	//ConnectionMonitor* cmEE = sim.setConnectionMonitor(gExc, gInh, "DEFAULT");

	//setup some baseline input
	PoissonRate in(N_EXC);
	in.setRates(1.0f);
	sim.setSpikeRate(gInput, &in);

	// run for a total of 10 seconds
	// at the end of each runNetwork call, SpikeMonitor stats will be printed
	//smInput->startRecording();
	//smExc->startRecording();
	//smInh->startRecording();
	//smInput->startRecording();
	
	sim.runNetwork(10, 0);
	
	//smInput->stopRecording();
	//smExc->stopRecording();
	//smInh->stopRecording();
	//smInput->stopRecording();

	//smExc->print(true);
	//smExc2->print(true);
	//smInput->print(true);

	return 0;
}

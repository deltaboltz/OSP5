/* Author:      Connor Schultz
 * Created:     November 16, 2020
 * Last Edit:   November 16, 2020
 */

//include of custom header files
#include "util.h"
#include "resource_handler.h"

void resman::stateclaim(int PID, int resclaim[20])//sets the max claim of a certain process
{
    //loops through the claim array to set all the values to what the process declaired
    for (int i : range(20))
    {
        this->desc[i].claim[PID] = resclaim[i];
    }

    started[PID] = true; //the process has now started
}

bool resman::isSafe() //checks to see if the request that is sent in will be safe (Deadlock Avoidance Alg)
{
    this->lastBlockTest.clear();
    int currentavail[20];

    //loop through the the availble resources in the system and save them locally to the algorithm
    for (int i : range(20))
    {
        currentavail[i] = this->desc[i].avail;
    }

    //array of how many processes are running at a given time
    bool running[18];

    //copy the running flags inside each process resource manager to save them locally for the algorithm
    for (int PID : range(18))
    {
        running[PID] = this->started[PID];
    }

    //beginning of Deadlock Avoidance Algorithm (logic can be found in class notes)
    while(1)
    {
        bool claimsatisfied = false; //assume our claim is not satisfied

        //loop through all pids
        for (int PID : range(18))
        {
            //if a pid is set to running
            if (running[PID])
            {
                claimsatisfied = true; //set the claim satisfied to true

                //loop through to check all C-A vectors are not greater than the available remaining in the system
                for (int descID : range(20))
                {
                    if (this->desc[descID].claim[PID] - this->desc[descID].alloc[PID] > currentavail[descID])
                    {
                      claimsatisfied = false; //if the C-A is too high then the claimsatisfied is now false
                    }
                }

                if (claimsatisfied) //if the claimsatisfied is still true from the above condition then
                {
                    //loop through to simulate the allocation to the process
                    for (int descID : range(20))
                    {
                      currentavail[descID] += this->desc[descID].alloc[PID];
                    }

                    //set the pid to not running anymore
                    running[PID] = false;
                    break; //continue simulation of allocations
                }
            }
        }
        if (!claimsatisfied)//if the claim is not satisfied then the state was unsafe to allocate
        {
            for (int PID : range(18))
            {
                if (running[PID] && this->started[PID])
                {
                    this->lastBlockTest.push_back(PID);
                }
            }
            return false;
        }

        bool nonerunning = true; //assume that no process is running

        //check to see if any process is running still
        //if none are running and continue simulation
        for (int PID : range(18))
        {
          nonerunning &= !running[PID];
        }

        if (nonerunning)
        {
          return true;
        }
    }
}

int resman::allocate(int PID, int descID)//overloaded function if the number requested is not stated by the process (default 1)
{
    return this->allocate(PID, descID, 1);
}

int resman::allocate(int PID, int descID, int instances) //allocates the number of a specific resource to the requesting process
{
    if (this->desc[descID].alloc[PID] + instances > this->desc[descID].claim[PID]) //checks to see if the claim is above the max claim by the process
    {
        customerrorquit("PID " + std::to_string(PID) + " requested " + std::to_string(instances) + " of R" + std::to_string(descID) + " but has a maximum claim of " + std::to_string(this->desc[descID].claim[PID]));
    }

    if (this->desc[descID].shareable) //checks to see of the resource is flagged as being a shareable resource
    {
        this->desc[descID].alloc[PID] += instances;
        return 0;
    }

    if (instances > this->desc[descID].avail) //checks to see if the requested value is greater than availble in the system (we know this won't work)
    {
        return 1; //return 1 to oss to block the child
    }
    else
    {
      //"allocates" the requested resource in order to run the simulation of deadlock
        this->desc[descID].alloc[PID] += instances;
        this->desc[descID].avail -= instances;

        if (this->isSafe()) //if the request is safe then we can continue on with the program
        {
            return 0;
        }
        else //if the request is unsafe then deallocate the possible request and return2 to oss to block the child
        {
            this->desc[descID].alloc[PID] -= instances;
            this->desc[descID].avail += instances;
            return 2;
        }
    }
}

void resman::release(int PID, int descID) //overloaded function to release resources
{
    this->release(PID, descID, 1);
}

void resman::release(int PID, int descID, int instances)//releases the resources sent by the specific child process
{
    this->desc[descID].alloc[PID] -= instances;

    if (!this->desc[descID].shareable)
    {
        this->desc[descID].avail += instances;
    }
}

int resman::findfirstunset() //finds the first unset bit in the bitmap
{
    for (int i : range(18)) if (!this->bitmap[i]) return i;

    return -1;
}

void resman::findandsetpid(int& pid) //set the pid inside the first unset bit in the bitmap (if we can)
{
    if ((pid = findfirstunset()) != -1)
    {
        this->bitmap.set(pid);
    }
}

void resman::unsetpid(int pid) //remove the pid from the bitmap to open up a new space
{
    this->bitmap.reset(pid);
    this->started[pid] = false;
}

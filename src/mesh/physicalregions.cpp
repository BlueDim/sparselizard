#include "physicalregions.h"
#include "universe.h"


physicalregions::physicalregions(disjointregions& inputdisjointregions)
{
    mydisjointregions = &inputdisjointregions;
}

int physicalregions::createunion(const std::vector<int> input)
{
    errorundefined(input);

    std::vector<int> disjregs = {};
    for (int i = 0; i < input.size(); i++)
    {
        // Get all disjoint regions with -1:
        std::vector<int> disjregsinthisphysreg = get(input[i])->getdisjointregions(-1);
        for (int j = 0; j < disjregsinthisphysreg.size(); j++)
            disjregs.push_back(disjregsinthisphysreg[j]);
    }
    int newphysregnum = getmaxphysicalregionnumber() + 1;
    
    physicalregion* newphysreg = get(newphysregnum);
    newphysreg->setdisjointregions(disjregs);
    
    return newphysregnum;
}

int physicalregions::createintersection(const std::vector<int> input)
{
    errorundefined(input);

    std::vector<int> disjregs = {};
    for (int i = 0; i < input.size(); i++)
    {
        // Get all disjoint regions with -1:
        std::vector<int> disjregsinthisphysreg = get(input[i])->getdisjointregions(-1);
        
        if (i > 0)
            disjregs = myalgorithm::intersect(disjregs, disjregsinthisphysreg);
        else
            disjregs = disjregsinthisphysreg;
    }
    int newphysregnum = getmaxphysicalregionnumber() + 1;
    
    physicalregion* newphysreg = get(newphysregnum);
    newphysreg->setdisjointregions(disjregs);
    
    return newphysregnum;
}

int physicalregions::createexclusion(int input, int toexclude)
{
    errorundefined({input,toexclude});

    // Get all input disjoint regions:
    std::vector<int> inputdisjregs = get(input)->getdisjointregions(-1);

    // Get all disjoint regions to exclude:
    std::vector<int> toexcludedisjregs = get(toexclude)->getdisjointregions(-1);

    // Remove the disjoint regions to exclude from the list of input disjoint regions:
    std::vector<int> disjregs = {};
    for (int i = 0; i < inputdisjregs.size(); i++)
    {
        bool toinclude = true;
        for (int j = 0; j < toexcludedisjregs.size(); j++)
        {
            if (inputdisjregs[i] == toexcludedisjregs[j])
            {
                toinclude = false;
                break;            
            }
        }
        if (toinclude)
            disjregs.push_back(inputdisjregs[i]);
    }

    int newphysregnum = getmaxphysicalregionnumber() + 1;
    
    physicalregion* newphysreg = get(newphysregnum);
    newphysreg->setdisjointregions(disjregs);
    
    return newphysregnum;
}

int physicalregions::createunionofall(void)
{
    int problemdimension = universe::mymesh->getmeshdimension();
    
    std::vector<int> tounite = {};
    
    // Get all regions of max dimension:
    for (int i = 0; i < myphysicalregionnumbers.size(); i++)
    {
        if (myphysicalregions[i]->getelementdimension() == problemdimension)
            tounite.push_back(myphysicalregionnumbers[i]);
    }

    return createunion(tounite);
}

int physicalregions::getmaxphysicalregionnumber(void)
{
    return *std::max_element(myphysicalregionnumbers.begin(), myphysicalregionnumbers.end());
}

physicalregion* physicalregions::get(int physicalregionnumber)
{        
    // Try to find the physical region number in 'myphysicalregionnumbers':
    for (int i = 0; i < myphysicalregionnumbers.size(); i++)
    {
        if (myphysicalregionnumbers[i] == physicalregionnumber)
            return myphysicalregions[i].get();
    }
    
    // If it could not be found append it:
    std::shared_ptr<physicalregion> newphysicalregion(new physicalregion(*mydisjointregions, *this, physicalregionnumber));
    myphysicalregions.push_back(newphysicalregion);
    myphysicalregionnumbers.push_back(physicalregionnumber);
    
    return (myphysicalregions[myphysicalregions.size()-1]).get();
}

physicalregion* physicalregions::getatindex(int physicalregionindex)
{
    return (myphysicalregions[physicalregionindex]).get();
}

int physicalregions::count(int dim)
{
    if (dim == -1)
        return myphysicalregionnumbers.size();
    else
    {
        int num = 0;
        for (int i = 0; i < myphysicalregionnumbers.size(); i++)
        {
            if (myphysicalregions[i]->getelementdimension() == dim)
                num++;
        }
        return num;
    }    
}

int physicalregions::countelements(void)
{
    int numelem = 0;
    for (int i = 0; i < myphysicalregions.size(); i++)
        numelem += myphysicalregions[i]->countelements();
    
    return numelem;
}

std::vector<int> physicalregions::getallnumbers(int dim)
{
    if (dim == -1)
        return myphysicalregionnumbers;
    else
    {
        std::vector<int> out(count(dim));
        int index = 0;
        for (int i = 0; i < myphysicalregions.size(); i++)
        {
            if (myphysicalregions[i]->getelementdimension() == dim)
            {   
                out[index] = myphysicalregionnumbers[i];
                index++;
            }
        }
        return out;
    }
}

int physicalregions::getnumber(int physicalregionindex)
{
    return myphysicalregionnumbers[physicalregionindex];
}

int physicalregions::getindex(int physicalregionnumber)
{
    for (int i = 0; i < myphysicalregionnumbers.size(); i++)
    {
        if (myphysicalregionnumbers[i] == physicalregionnumber)
            return i;
    }
    return -1;
}

void physicalregions::remove(std::vector<int> toremove, bool ispartofdisjregstructure)
{
    // Tag regions to remove:
    std::vector<bool> istoremove(myphysicalregionnumbers.size(), false);

    for (int i = 0; i < toremove.size(); i++)
    {
        int curindex = getindex(toremove[i]);
        if (curindex != -1)
            istoremove[curindex] = true;
    }

    int index = 0;
    for (int i = 0; i < myphysicalregionnumbers.size(); i++)
    {
        if (not(istoremove[i]))
        {
            myphysicalregions[index] = myphysicalregions[i];
            myphysicalregionnumbers[index] = myphysicalregionnumbers[i];
            
            index++;
        }
    }
    myphysicalregions.resize(index); 
    myphysicalregionnumbers.resize(index);
    
    if (ispartofdisjregstructure)
        mydisjointregions->removephysicalregions(istoremove);
}

void physicalregions::errorundefined(std::vector<int> physregs)
{
    for (int i = 0; i < physregs.size(); i++)
    {
        if (getindex(physregs[i]) == -1)
        {
            std::cout << "Error in 'physicalregions' object: physical region number " << physregs[i] << " is not defined" << std::endl;
            abort();
        }
    }
}

void physicalregions::clear(void)
{
    std::vector<std::shared_ptr<physicalregion>> myphysicalregions = {};
    std::vector<int> myphysicalregionnumbers = {};
}


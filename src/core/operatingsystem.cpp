#include "operatingsystem.h"
#include "scheduler.h"
#include <assert.h>

OperatingSystem::OperatingSystem(
    int quantum,
    int overhead,
    SchedulingAlgorithm schedulingAlgorithm,
    PageReplacementAlgorithm pra)
:   m_quantum(quantum),
    m_overhead(overhead),
    m_time(0),
    m_numberOfProccesses(0)
{
    m_scheduler = new Scheduler(quantum, overhead, schedulingAlgorithm);
    m_memoryManager = new MemoryManager(pra);
}

OperatingSystem::~OperatingSystem()
{
    delete m_scheduler;
    m_scheduler = nullptr;

    // Remove proccesses from memory
    for(auto proccess = m_proccesses.begin();
        proccess != m_proccesses.end();)
    {
        delete *proccess;
        m_proccesses.erase(proccess);
    }

    for(auto proccess = m_executionQueue.begin();
        proccess != m_executionQueue.end();)
    {
        delete *proccess;
        m_executionQueue.erase(proccess);
    }

    for(auto proccess = m_finishedProccesses.begin();
        proccess != m_finishedProccesses.end();)
    {
        delete *proccess;
        m_finishedProccesses.erase(proccess);
    }

    delete m_memoryManager;
    m_memoryManager = nullptr;
}

SchedulingAlgorithm OperatingSystem::GetSchedulingAlgorithm() const
{
    return m_scheduler->GetSchedulingAlgorithm();
}

const MemoryManager* OperatingSystem::GetMemoryManager() const
{
    #ifdef M_DEBUG
    assert(m_memoryManager != nullptr);
    #endif

    return m_memoryManager;
}

float OperatingSystem::GetAverageTurnaround() const
{
    // No proccess has ever been finished
    if(m_finishedProccesses.size() == 0) return 0;

    int totalExecutionTime = 0;
    int numberOfFinishedProccessses = m_finishedProccesses.size();

    for(auto proccess = m_finishedProccesses.begin();
        proccess != m_finishedProccesses.end();
        proccess++)
    {
        totalExecutionTime += (*proccess)->GetTurnAround();
    }

    return (float)totalExecutionTime / numberOfFinishedProccessses;
}

int OperatingSystem::WhichProccessOwnsThisPage(unsigned page) const
{
    #ifdef M_DEBUG
    assert(page < m_pageMap.size());
    #endif

    return m_pageMap[page];
}

void OperatingSystem::AddProccess(
    int id, 
    unsigned arrivalTime, 
    unsigned duration, 
    unsigned deadline,
    unsigned numberOfPages)
{
    m_proccesses.push_back(
        new Proccess(id, arrivalTime, duration, deadline, numberOfPages));
    
    // Set ID, so proccesses IDs have no gaps
    m_proccesses.back()->SetID(m_proccesses.size());
    m_numberOfProccesses++;

    // Resize the page map to accomodate the new proccess
    m_pageMap.resize( m_pageMap.size() + numberOfPages);
}

bool OperatingSystem::NextStep()
{
    // If there are no proccesses in execution,
    // no blocked proccesses,
    // and no proccesses to arrive
    // there is nothing to be done.
    if(m_executionQueue.size()   == 0 && 
       m_proccesses.size()       == 0 &&
       m_blockedProcesses.size() == 0) 
       return false;

    UpdateExecutionQueue();

    // Advances time
    m_time++;    

    // There are proccesses active at the moment
    // Note(Gustavo): This verification is needed
    // because it's possible that the execution queue
    // is empty (no active proccesses at the moment),
    // but proccesses will arrive later.
    if(!m_executionQueue.empty() || !m_blockedProcesses.empty())
    {
        m_scheduler->Run(
            &m_executionQueue, 
            &m_finishedProccesses,
            &m_blockedProcesses,
            m_memoryManager,
            m_time);

        // if(m_executionQueue.size() > 0) m_executionQueue.front()->Run(m_time);
    }     
    
    m_memoryManager->Tick();
    return true;
}

void OperatingSystem::Reset()
{
    m_scheduler->Reset();

    // Put current running proccesses 
    // back in waiting list.
    for(auto p = m_executionQueue.begin();
        p != m_executionQueue.end();)
    {
        m_proccesses.push_back(*p);
        m_executionQueue.erase(p);
    }

    // Put finished proccesses back in
    // waiting list.
    for(auto p = m_finishedProccesses.begin();
        p != m_finishedProccesses.end();)
    {
        m_proccesses.push_back(*p);
        m_finishedProccesses.erase(p);
    }

    // Reset all proccesses
    for(auto p = m_proccesses.begin();
        p != m_proccesses.end();
        p++)
    {
        (*p)->Reset();
    }
}

void OperatingSystem::UpdateExecutionQueue()
{
    for(auto proccess = m_proccesses.begin(); 
        proccess != m_proccesses.end();)
    {
        if((*proccess)->GetArrivalTime() == m_time)
        {
            // Reserve pages
            std::vector<unsigned> reservedPages = 
                m_memoryManager->ReserveVirtualPages(
                    (*proccess)->GetNumberOfPages());

            // Map the pages to the new project in page map
            for(auto& page : reservedPages)
            {
                m_pageMap[page] = (*proccess)->GetID();
            }

            (*proccess)->SetPages(reservedPages);
            
            m_executionQueue.push_back(*proccess);
            m_proccesses.erase(proccess);


        }
        else 
        {
            proccess++;
        }
    }
}
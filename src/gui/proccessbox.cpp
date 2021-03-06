#include "proccessbox.h"

ProccessBox::ProccessBox(unsigned proccessIndex, QWidget* parent)
: QGroupBox(tr("Proccess %1").arg(proccessIndex), parent),
    m_ID(proccessIndex)
{
    CreateSpinBoxes();
    CreateRemoveButton();
    CreateMainLayout();
}

void ProccessBox::SetArrivalTime(unsigned value)
{
	m_arrivalTimeInput->setValue(value); 
}

void ProccessBox::SetDuration(unsigned value)
{
	m_durationInput->setValue(value);
}

void ProccessBox::SetDeadline(unsigned value)
{
	m_deadlineInput->setValue(value); 
}

void ProccessBox::SetNumberOfPages(unsigned nPages)
{
    // Check if pages are within the accepted range
    if(nPages < MIN_NUMBER_OF_PAGES || nPages > MAX_NUMBER_OF_PAGES) return;

    m_nPagesInput->setValue(nPages);
}

void ProccessBox::CreateSpinBoxes()
{
    m_arrivalTimeLabel = new QLabel(tr("Arrival Time"), this);
    m_arrivalTimeInput = new QSpinBox(this);
    m_arrivalTimeInput->setMaximum(99999);
    m_arrivalTimeInput->setStatusTip(tr("Set the arrival time of the proccess."));

    m_durationLabel = new QLabel(tr("Exec. Time"), this);
    m_durationInput = new QSpinBox(this);
    m_durationInput->setMinimum(1);
    m_durationInput->setMaximum(99999);
    m_durationInput->setStatusTip(tr("Set the duration of the proccess."));

    m_deadlineLabel = new QLabel(tr("Deadline"), this);
    m_deadlineInput = new QSpinBox(this);
    m_deadlineInput->setMaximum(99999);
    m_deadlineInput->setStatusTip(tr("Set the priority for the proccess."));

    m_nPagesLabel = new QLabel(tr("Pages"), this);
    m_nPagesInput = new QSpinBox(this);
    m_nPagesInput->setMinimum(MIN_NUMBER_OF_PAGES);
    m_nPagesInput->setMaximum(MAX_NUMBER_OF_PAGES);
    m_nPagesInput->setStatusTip(tr("Set the number of pages the proccess will use."));
}

void ProccessBox::CreateRemoveButton()
{
    m_removeButton = new QPushButton(tr("Remove"), this);
    connect(m_removeButton, &QPushButton::clicked, 
        this, &ProccessBox::RemoveButtonClicked);
}

void ProccessBox::CreateMainLayout()
{
    m_proccessLayout = new QGridLayout(this);
    m_proccessLayout->addWidget(m_arrivalTimeLabel, 0, 0);
    m_proccessLayout->addWidget(m_arrivalTimeInput, 0, 1);
    m_proccessLayout->addWidget(m_durationLabel, 1, 0);
    m_proccessLayout->addWidget(m_durationInput, 1, 1);
    m_proccessLayout->addWidget(m_deadlineLabel, 2, 0);
    m_proccessLayout->addWidget(m_deadlineInput, 2, 1);
    m_proccessLayout->addWidget(m_nPagesLabel, 3, 0);
    m_proccessLayout->addWidget(m_nPagesInput, 3, 1);
    m_proccessLayout->addWidget(m_removeButton, 4, 1, Qt::AlignRight);

    setFixedSize(150, 150);
    setLayout(m_proccessLayout);
}

unsigned ProccessBox::GetArrivalTime()
{
    return (unsigned)m_arrivalTimeInput->value();
}

unsigned ProccessBox::GetDuration()
{
    return (unsigned)m_durationInput->value();
}

unsigned ProccessBox::GetDeadline()
{
    return (unsigned)m_deadlineInput->value();
}

unsigned ProccessBox::GetNumberOfPages()
{
    return (unsigned)m_nPagesInput->value();
}

void ProccessBox::RemoveButtonClicked()
{
    emit RemoveProccessRequested(this);
}

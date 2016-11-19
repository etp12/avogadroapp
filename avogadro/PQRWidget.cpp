#include "PQRWidget.h"
#include "ImportPQR.h"
#include "ui_PQRWidget.h"

namespace Avogadro {

PQRWidget::PQRWidget(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PQRWidget)
{
  ui->setupUi(this);
  connect(ui->searchButton, SIGNAL(clicked(bool)), this, SLOT(searchAction()));
	connect(ui->downloadButton, SIGNAL(clicked(bool)), this, SLOT(downloadMol()));

	ui->tableWidget->setColumnCount(3);
	ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Name" << "Formula" << "Mass");
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int, int)),
		this, SLOT(molSelected(int, int)));

  request = new ImportPQR(ui->tableWidget, ui->svgPreview, ui->filename, ui->nameDisplay, ui->formulaDisplay);
}

PQRWidget::~PQRWidget()
{
    delete ui;
    delete request;
}

void PQRWidget::searchAction()
{
    QString url = "https://pqr.pitt.edu/api/browse/"+ui->molName->text() + "/" + ui->searchTypeBox->currentText();
    request->sendRequest(url);
}

void PQRWidget::molSelected(int row, int col)
{
	currentlySelectedMol = request->molSelected(row);
}

void PQRWidget::downloadMol() {
	QString mol2url = currentlySelectedMol;
	if (mol2url != "N/A" && mol2url != "") {
		QString ext = ui->extensionType->currentText();
		if (ext == "mol2") {
			ext = "mol"; //easiest workaround to PQR api using /mol not /mol2
		}
		if (ext == "mol" || ext == "json") {
			mol2url.remove(0, 3); //remove first 3 characters to map to PQR's url
			QString url = "https://pqr.pitt.edu/api/" + ext + "/" + mol2url;
			request->sendRequest(url, mol2url, ui->downloadFolder->text(), "."+ext);
		}
		else if (ext == "svg") {
			QString url = "https://pqr.pitt.edu/static/data/svg/"+ mol2url + ".svg";
			request->sendRequest(url, mol2url.remove(0, 3), ui->downloadFolder->text(), "." + ext);
		}
	}
}

}

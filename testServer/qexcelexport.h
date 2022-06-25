#ifndef QEXCELEXPORT_H
#define QEXCELEXPORT_H


//ExcelExport.h
#ifndef EXCELEXPORT_H
#define EXCELEXPORT_H
#include <QAxObject>
#include <QDir>
#include <QFile>
class QExcelExport
{
public:
    QExcelExport();

    void newExcel(const QString &fileName);
    void appendSheet(const QString &sheetName);
    void setCellValue(int row, int column, const QString &value);
    void saveExcel(const QString &fileName);
    void freeExcel();
private:
    QAxObject *pApplication;
    QAxObject *pWorkBooks;
    QAxObject *pWorkBook;
    QAxObject *pSheets;
    QAxObject *pSheet;
};
#endif

#endif // QEXCELEXPORT_H

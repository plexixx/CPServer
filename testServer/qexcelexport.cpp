#include "qexcelexport.h"

QExcelExport::QExcelExport()
{

}

//新建Execl文件
void QExcelExport::newExcel(const QString &fileName)
{
    pApplication = new QAxObject();
    pApplication->setControl("Excel.Application"); //连接Excel控件
    pApplication->dynamicCall("SetVisible(bool)", false); //false不显示窗体
    pApplication->setProperty("DisplayAlerts", false); //不显示任何警告信息。
    pWorkBooks = pApplication->querySubObject("Workbooks");
    QFile file(fileName);
    if(file.exists())
      {
       pWorkBook = pWorkBooks->querySubObject("Open(const QString &)", fileName);
    }
    else
      {
       pWorkBooks->dynamicCall("Add");
       pWorkBook = pApplication->querySubObject("ActiveWorkBook");
    }
    //默认有一个sheet
    pSheets = pWorkBook->querySubObject("Sheets");
    pSheet = pSheets->querySubObject("Item(int)", 1);
}

//增加1个Worksheet，需要两个以上sheet时调用，默认有一个
void QExcelExport::appendSheet(const QString &sheetName)
{
    int cnt = 1;
    QAxObject *pLastSheet = pSheets->querySubObject("Item(int)", cnt);
    pSheets->querySubObject("Add(QVariant)", pLastSheet->asVariant());
    pSheet = pSheets->querySubObject("Item(int)", cnt);
    pLastSheet->dynamicCall("Move(QVariant)", pSheet->asVariant());
    pSheet->setProperty("Name", sheetName);
}

//向Excel单元格中写入数据,可以设置单元格的属性
void QExcelExport::setCellValue(int row, int column, const QString &value)
{
    QAxObject *pRange = pSheet->querySubObject("Cells(int,int)", row, column);
    pRange->dynamicCall("Value", value);
    //内容居中
    pRange->setProperty("HorizontalAlignment", -4108);
    pRange->setProperty("VerticalAlignment", -4108);
     pRange->setProperty("RowHeight", 50); //设置单元格行高
     pRange->setProperty("ColumnWidth", 30); //设置单元格列宽

     pRange->setProperty("WrapText", true); //内容过多，自动换行
    pRange->dynamicCall("ClearContents()"); //清空单元格内容

     QAxObject* interior =pRange->querySubObject("Interior");
    //interior->setProperty("Color", QColor(0, 255, 0)); //设置单元格背景色（绿色）
     //QAxObject* border = cell->querySubObject("Borders");
     //border->setProperty("Color", QColor(0, 0, 255)); //设置单元格边框色（蓝色）
    // QAxObject *font = cell->querySubObject("Font"); //获取单元格字体
//    font->setProperty("Name", QStringLiteral("华文彩云")); //设置单元格字体
//     font->setProperty("Bold", true); //设置单元格字体加粗
//     font->setProperty("Size", 20); //设置单元格字体大小
//     font->setProperty("Italic", true); //设置单元格字体斜体
//     font->setProperty("Underline", 2); //设置单元格下划线
    //font->setProperty("Color", QColor(255, 0, 0)); //设置单元格字体颜色（红色）

      if(row == 1)
      {
      QAxObject *font = pRange->querySubObject("Font"); //获取单元格字体
      font->setProperty("Bold",true);
    }
}

//保存Excel
void QExcelExport::saveExcel(const QString &fileName)
{
   pWorkBook->dynamicCall("SaveAs(const QString &)",
   QDir::toNativeSeparators(fileName));
}

////释放Excel
//void ExcelHelper::freeExcel()
//{
//   if (pApplication != NULL)
//     {
//     pApplication->dynamicCall("Quit()");
//     delete pApplication;
//     pApplication = NULL;
//   }
//}

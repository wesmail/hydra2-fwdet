#ifndef HDBCOLUMN
#define HDBCOLUMN

#include "TNamed.h"

class HDbColumnType; 

class HDbColumn : public TNamed {
private:
  HDbColumnType* colType;      // Type of the column
  Int_t          colId;        // Position index of the column in an existing table
  Bool_t         colNullable;  // Nullability of the column
public:
  HDbColumn();
  HDbColumn(const Char_t*);
  HDbColumn(HDbColumn&);
  ~HDbColumn();
  void setColumnId(Int_t n) { colId=n; }
  void setNullable(Bool_t status=kTRUE) { colNullable=status; }
  void setColType(const Char_t*, Int_t, Int_t p=0, Int_t s=0);
  void setColType(const Char_t*);
  void copyColumn(HDbColumn*);
  HDbColumnType* getColType() { return colType; }
  Int_t getColumnId() { return colId; }
  Bool_t isNullable() { return colNullable; }
  void show();
  ClassDef(HDbColumn,0) // Class for a column of an Oracle table
};

#endif /* !HDBCOLUMN_H */

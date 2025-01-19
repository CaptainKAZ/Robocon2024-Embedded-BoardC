import gdb
from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QTableWidget,
    QTableWidgetItem,
    QPushButton,
    QVBoxLayout,
    QWidget,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QComboBox,
    QHeaderView,
)
import sys


def parseType(type):
    """Convert GDB type to our type enum"""
    if type.code == gdb.TYPE_CODE_ENUM:
        return 1
    if type.code == gdb.TYPE_CODE_CHAR:
        return 1
    if type.code == gdb.TYPE_CODE_INT and type.sizeof == 1 and not type.is_signed:
        return 1
    if type.code == gdb.TYPE_CODE_INT and type.sizeof == 2 and not type.is_signed:
        return 2
    if type.code == gdb.TYPE_CODE_INT and type.sizeof == 4 and not type.is_signed:
        return 3
    if type.code == gdb.TYPE_CODE_INT and type.sizeof == 1 and type.is_signed:
        return 4
    if type.code == gdb.TYPE_CODE_INT and type.sizeof == 2 and type.is_signed:
        return 5
    if type.code == gdb.TYPE_CODE_INT and type.sizeof == 4 and type.is_signed:
        return 6
    if type.code == gdb.TYPE_CODE_FLT and type.sizeof == 4:
        return 7
    if type.code == gdb.TYPE_CODE_FLT and type.sizeof == 8:
        return 8
    return 0


class AresGdbGui(QMainWindow):
    def __init__(self):
        super().__init__()
        self.gdbReqSymbol = "aresGdbPlotReq"
        reqArray = gdb.parse_and_eval(self.gdbReqSymbol)
        struct_type = reqArray.type.target()
        array_size = reqArray.type.sizeof // struct_type.sizeof
        print("there are {} available plot lines".format(array_size))
        self.reqArrayLen = array_size
        self.reqArray = reqArray
        self.initUI()

    def initUI(self):
        self.setWindowTitle("ARES GDB GUI")
        self.setGeometry(100, 100, 800, 600)

        # Main layout
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        layout = QVBoxLayout(main_widget)

        # Variable selection
        input_layout = QVBoxLayout()

        # Variable selection controls
        input_layout.addWidget(QLabel("Variable Selection:"))

        # Create a horizontal layout for variable controls
        var_controls_layout = QHBoxLayout()

        # Variable name dropdown
        var_controls_layout.addWidget(QLabel("Variable Name:"))
        self.var_combo = QComboBox()
        self.var_combo.setEditable(True)
        self.var_combo.setInsertPolicy(QComboBox.NoInsert)
        self.var_combo.currentTextChanged.connect(self.update_var_info)
        var_controls_layout.addWidget(self.var_combo)

        # Type dropdown
        var_controls_layout.addWidget(QLabel("Type:"))
        self.type_combo = QComboBox()
        self.type_combo.addItems(
            [
                "TYPE_UNKNOWN",
                "TYPE_U8",
                "TYPE_U16",
                "TYPE_U32",
                "TYPE_I8",
                "TYPE_I16",
                "TYPE_I32",
                "TYPE_F32",
                "TYPE_F64",
            ]
        )
        var_controls_layout.addWidget(self.type_combo)

        # Address input
        var_controls_layout.addWidget(QLabel("Address:"))
        self.addr_input = QLineEdit()
        font_metrics = self.addr_input.fontMetrics()
        char_width = font_metrics.horizontalAdvance("0")
        self.addr_input.setMinimumWidth(char_width * 10 + 5)
        var_controls_layout.addWidget(self.addr_input)

        # Add variable controls to main layout
        input_layout.addLayout(var_controls_layout)

        # Create a horizontal layout for buttons
        button_layout = QHBoxLayout()

        # Refresh button
        refresh_button = QPushButton("Refresh Variables")
        refresh_button.clicked.connect(self.populate_variables)
        button_layout.addWidget(refresh_button)

        # Add variable button
        add_button = QPushButton("Add Variable")
        add_button.clicked.connect(self.add_variable)
        button_layout.addWidget(add_button)

        # Clear variables button
        clear_button = QPushButton("Clear Variables")
        clear_button.clicked.connect(self.clear_variables)
        button_layout.addWidget(clear_button)

        # Add buttons to main layout
        input_layout.addLayout(button_layout)

        # Populate initial variables
        self.populate_variables()

        # Table for displaying values
        self.table = QTableWidget(0, 3)
        self.table.setHorizontalHeaderLabels(["Variable", "Address", "TYPE"])
        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeToContents)
        self.table.horizontalHeader().setStretchLastSection(True)

        # Add all to main layout
        layout.addLayout(input_layout)
        layout.addWidget(self.table)

    def _is_primitive_type(self, type_code):
        """Check if type is primitive (int, float, etc.)"""
        return type_code in (
            gdb.TYPE_CODE_INT,
            gdb.TYPE_CODE_FLT,
            gdb.TYPE_CODE_CHAR,
            gdb.TYPE_CODE_ENUM,
        )

    def _should_skip_type(self, type_str):
        """Check if type should be skipped (const or pointer)"""
        return "const" in type_str.lower() or "*" in type_str

    def _process_variable(self, var_name, var_value, variables, prefix=""):
        """Recursively process a variable"""
        try:
            type_code = var_value.type.strip_typedefs().code
            type_str = str(var_value.type.strip_typedefs())

            if self._should_skip_type(type_str):
                return

            if self._is_primitive_type(type_code):
                address = hex(int(var_value.address))
                variables.append(
                    (f"{prefix}{var_name}", f"{prefix}{var_name}", type_str, address)
                )
                return

            if type_code == gdb.TYPE_CODE_ARRAY:
                array_size = int(var_value.type.range()[1]) + 1
                elem_type = var_value.type.target().strip_typedefs()

                if elem_type.code in (gdb.TYPE_CODE_STRUCT, gdb.TYPE_CODE_UNION):
                    try:
                        array_size = int(var_value.type.range()[1]) + 1
                        for i in range(array_size):
                            if i >= array_size:
                                print(
                                    f"[WARNING] Index {i} out of bounds for array {var_name}"
                                )
                                continue
                            elem_name = f"{var_name}[{i}]"
                            try:
                                elem_value = var_value[i]
                                for field in elem_type.fields():
                                    field_name = f"{elem_name}.{field.name}"
                                    try:
                                        field_value = elem_value[field.name]
                                        if "*" in str(field_value.type):
                                            continue
                                        self._process_variable(
                                            field_name, field_value, variables, prefix
                                        )
                                    except Exception as e:
                                        print(
                                            f"[WARNING] Error processing field {field_name}: {e}"
                                        )
                            except Exception as e:
                                print(
                                    f"[WARNING] Error processing array element {elem_name}: {e}"
                                )
                    except Exception as e:
                        print(f"[ERROR] Error processing array {var_name}: {e}")
                    return

                for i in range(array_size):
                    elem_name = f"{var_name}[{i}]"
                    elem_value = var_value[i]
                    self._process_variable(elem_name, elem_value, variables, prefix)
                return

            if type_code in (gdb.TYPE_CODE_STRUCT, gdb.TYPE_CODE_UNION):
                try:
                    for field in var_value.type.fields():
                        field_name = f"{var_name}.{field.name}"
                        try:
                            field_value = var_value[field.name]
                            if "*" in str(field_value.type):
                                continue
                            self._process_variable(
                                field_name, field_value, variables, prefix
                            )
                        except Exception as e:
                            print(f"[WARNING] Error processing field {field_name}: {e}")
                except Exception as e:
                    print(f"[ERROR] Error processing struct {var_name}: {e}")
                return

        except Exception as e:
            pass

    def populate_variables(self):
        """Get available variables from GDB and populate the dropdown"""
        self.var_combo.clear()
        try:
            output = gdb.execute("info variables", to_string=True)
            variables = []
            current_file = None

            for line in output.splitlines():
                if not line or line.startswith("All defined"):
                    continue

                if line.startswith("File "):
                    current_file = line.split("File ")[1].strip()
                    continue

                try:
                    if ":" in line:
                        line = line.split(":", 1)[1].strip()

                    parts = [p.strip() for p in line.split(";") if p.strip()]
                    if not parts:
                        continue

                    decl = parts[0]
                    decl_parts = decl.split()

                    if decl_parts[0].startswith("0x"):
                        var_type = " ".join(decl_parts[1:-1])
                        var_name = decl_parts[-1].rstrip(";")
                        var_name = var_name.split("[")[0]
                    else:
                        if decl_parts[0] == "static":
                            decl_parts = decl_parts[1:]
                        var_type = " ".join(decl_parts[:-1])
                        var_name = decl_parts[-1].rstrip(";")
                        var_name = var_name.split("[")[0]

                    if self._should_skip_type(var_type):
                        continue

                    try:
                        value = gdb.parse_and_eval(var_name)
                        self._process_variable(var_name, value, variables)
                    except Exception as e:
                        pass

                except Exception as e:
                    continue

            for var_name, _, _, _ in sorted(variables, key=lambda x: x[1]):
                self.var_combo.addItem(var_name)

        except Exception as e:
            print(f"Error getting variables: {e}")

    def update_var_info(self, var_name):
        """Update type and address when variable is selected"""
        if not var_name or len(var_name) < 2:
            self.type_combo.setCurrentIndex(0)
            self.addr_input.clear()
            return

        try:
            value = gdb.parse_and_eval(var_name)
            address = hex(int(value.address))
            type_code = parseType(value.type.strip_typedefs())
            self.type_combo.setCurrentIndex(type_code)
            self.addr_input.setText(address)
        except gdb.error as e:
            self.type_combo.setCurrentIndex(0)
            self.addr_input.clear()
        except Exception as e:
            print(f"Error updating variable info: {e}")

    def storeRequest(self, name, address, type):
        for i in range(self.reqArrayLen):
            reqElement = self.reqArray[i]
            if int(reqElement["address"]) == 0:
                try:
                    p = reqElement["name"].address
                    command = f'set {{char [32]}} {int(p)} = "{name[-31:]}"'
                    gdb.execute(command)

                    p = reqElement["type"].address
                    command = f"set {{unsigned char}} {int(p)} = {parseType(type)}"
                    gdb.execute(command)

                    p = reqElement["address"].address
                    command = f"set {{unsigned long}} {int(p)} = {address}"
                    gdb.execute(command)

                    break
                except gdb.error as e:
                    continue

    def add_variable(self):
        var_name = self.var_combo.currentText()
        if var_name:
            try:
                # Check if array is full
                if self.table.rowCount() >= self.reqArrayLen:
                    print(f"Cannot add more than {self.reqArrayLen} variables")
                    return

                address = self.addr_input.text()
                row = self.table.rowCount()
                self.table.insertRow(row)
                display_name = var_name

                self.table.setItem(row, 0, QTableWidgetItem(display_name))
                self.table.setItem(row, 1, QTableWidgetItem(address))
                self.table.setItem(
                    row, 2, QTableWidgetItem(self.type_combo.currentText())
                )

                self.storeRequest(display_name, address=address, type=self.type_combo.currentIndex())
            except Exception as e:
                print(f"Error adding variable {var_name}: {e}")

    def clear_variables(self):
        """Clear all variables from table and array"""
        # Clear table
        self.table.setRowCount(0)
        
        # Clear array in GDB
        for i in range(self.reqArrayLen):
            reqElement = self.reqArray[i]
            try:
                # Clear name
                p = reqElement["name"].address
                command = f'set {{char [32]}} {int(p)} = ""'
                gdb.execute(command)

                # Clear type
                p = reqElement["type"].address
                command = f"set {{unsigned char}} {int(p)} = 0"
                gdb.execute(command)

                # Clear address
                p = reqElement["address"].address
                command = f"set {{unsigned long}} {int(p)} = 0"
                gdb.execute(command)
            except gdb.error as e:
                continue

    def update_values(self):
        for row in range(self.table.rowCount()):
            var_name = self.table.item(row, 0).text()
            try:
                value = gdb.parse_and_eval(var_name)
                address = value.address
                self.table.setItem(row, 2, QTableWidgetItem(str(value)))

                if "[" in var_name and "]" in var_name:
                    base_name = var_name.split("[")[0]
                    index = var_name.split("[")[1].split("]")[0]
                    array_value = gdb.parse_and_eval(base_name)
                    if array_value.type.code == gdb.TYPE_CODE_ARRAY:
                        array_size = int(array_value.type.range()[1]) + 1
                    var_type = self.type_combo.currentText()
                    self.table.setItem(
                        row,
                        0,
                        QTableWidgetItem(
                            f"{base_name}[{index} of {array_size}] {var_type} @ {hex(int(address))}"
                        ),
                    )
            except Exception as e:
                print(f"Error updating variable {var_name}: {e}")

class AresGdbGuiCommand(gdb.Command):
    def __init__(self):
        super(AresGdbGuiCommand, self).__init__("plot-gui", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        app = QApplication(sys.argv)
        window = AresGdbGui()
        window.show()
        app.exec_()

AresGdbGuiCommand()

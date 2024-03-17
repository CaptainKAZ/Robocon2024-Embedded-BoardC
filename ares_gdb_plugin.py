import gdb


class PrintExpressionCommand(gdb.Command):
    """
    typedef struct AresGdbPlotRequest {
      void *address;
      enum {
          TYPE_UNKNOWN,
          TYPE_U8,
          TYPE_U16,
          TYPE_U32,
          TYPE_I8,
          TYPE_I16,
          TYPE_I32,
          TYPE_F32,
          TYPE_F64
        } type;
      char name[32];
    } AresGdbPlotRequest;
    AresGdbPlotRequest aresGdbPlotReq[CONFIG_MAX_PLOTS];
    """

    def __init__(self):
        super(PrintExpressionCommand, self).__init__("plot", gdb.COMMAND_USER)
        self.gdbReqSymbol = "aresGdbPlotReq"
        reqArray = gdb.parse_and_eval(self.gdbReqSymbol)
        struct_type = reqArray.type.target()
        array_size = reqArray.type.sizeof // struct_type.sizeof
        print("there are {} available plot lines".format(array_size))
        self.reqArrayLen = array_size
        self.reqArray = reqArray

    def parseType(self, type):
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

    def storeRequest(self, name, address, type):
        for i in range(self.reqArrayLen):
            reqElement = self.reqArray[i]
            if int(reqElement["address"]) == 0:
                p = reqElement["name"].address
                command = f'set {{char [32]}} {int(p)} = "{name[-31:]}"'
                gdb.execute(command)
                p = reqElement["type"].address
                command = f'set {{unsigned char}} {int(p)} = {self.parseType(type)}'
                gdb.execute(command)
                p = reqElement["address"].address
                command = f'set {{unsigned long}} {int(p)} = {address}'
                gdb.execute(command)
                break
        for i in range(self.reqArrayLen):
            reqElement = self.reqArray[i]
            print(reqElement)

    def invoke(self, arg, from_tty):
        try:
            # 获取表达式的值
            value = gdb.parse_and_eval(arg)

            # 获取表达式的地址
            address = int(value.address)

            # 获取表达式的数据类型
            data_type = value.type.strip_typedefs()

            # 打印结果
            print("Expression: {}".format(arg))
            print("Address: {}".format(hex(address)))
            print(
                "Data Type: {} which is {}".format(
                    data_type, self.parseType(data_type)
                )
            )
            self.storeRequest(arg, address, data_type)
        except gdb.error as e:
            print("Error: {}".format(e))


PrintExpressionCommand()

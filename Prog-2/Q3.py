operators = {"&": "\\&", "++": "++", "+": "+", "-": "-", "*": "*", "<": "\\<", ">": "\\>", "=": "\\="}
pre_symbols = {' ', '|', '-', '`'}

def _split(s):
    res = []
    escape = False
    cur = []
    for ch in s:
        if ch == '\'':
            escape = not escape
        elif ch == ' ' and not escape and len(cur) > 0:
            res.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    if len(cur) > 0:
        res.append("".join(cur))
    return res


def to_node(line):
    if len(line) == 0:
        return None, None
    start = 0
    for start in range(len(line)):
        if line[start] not in pre_symbols:
            break
    level = start // 2
    details = _split(line[start:])
    # details = line[start:].split(' ')
    if len(details) >= 2:
        value = ""
        if "Operator" in details[0]:
            for ops, ops_to_print in operators.items():
                if ops in line:
                    value = details[0] + ", " + ops_to_print
                    break
        elif details[0] == "DeclRefExpr":
            value = details[0] + ", " + details[-2]
        elif details[0] == "IntegerLiteral":
            value = details[0] + ", " + details[-1]
        else:
            value = details[0]
        return TreeNode("Node" + details[1], value), level
    # elif len(details) == 1:
    #     if details[0] == "<<<NULL>>>":
    #         return TreeNode("Node0x0", "\\<NULL\\>"), level
    return None, None


class TreeNode:
    def __init__(self, key, value):
        self.key = key
        self.value = value
        self.children = []


def construct_tree(filepath: str):
    root = TreeNode("", "")
    stack = [root]
    prev = root
    with open(filepath) as fp:
        for line in fp:
            node, level = to_node(line.strip())
            if not node:
                continue
            if level == len(stack):
                pass
            elif level == len(stack) + 1:
                stack.append(prev)
            elif level <= len(stack) - 1:
                while len(stack) > level:
                    stack.pop()
            else:
                raise RuntimeError("parse error")
            stack[-1].children.append(node)
            prev = node
    return root.children[0]


def construct_dot(root: TreeNode):
    graph = []
    graph.append("digraph ast {")

    def traverse(node: TreeNode):
        if not node:
            return
        graph.append(f"\t{node.key} [shape=record, label = \"{{{node.value}}}\"];")
        for child in node.children:
            graph.append(f"\t{node.key} -> {child.key};")
        for child in node.children:
            traverse(child)

    traverse(root)
    graph.append("}")
    return graph


def write_to_file(graph):
    out_file = open("my_ast.txt", "w")
    for line in graph:
        # write line to output file
        out_file.write(line)
        out_file.write("\n")
    out_file.close()


filepath = "bubble_sort_ast.txt"
root = construct_tree(filepath)
graph = construct_dot(root)
write_to_file(graph)

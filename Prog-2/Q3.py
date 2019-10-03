def to_node(line):
    if len(line) == 0:
        return None, None
    start = 0
    for start in range(len(line)):
        if line[start] not in pre_symbols:
            break
    level = start // 2
    details = line[start:].split(' ')
    if len(details) >= 2:
        return TreeNode("Node" + details[1], details[0]), level
    # elif len(details) == 1:
    #     if details[0] == "<<<NULL>>>":
    #         return TreeNode("Node0x0", "\\<NULL\\>"), level
    return None, None

class TreeNode:
    def __init__(self, key, label):
        self.key = key
        self.label = label
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
        graph.append(f"\t{node.key} [shape=record, label = \"{{{node.label}}}\"];")
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
pre_symbols = {' ', '|', '-', '`'}
root = construct_tree(filepath)
graph = construct_dot(root)
write_to_file(graph)

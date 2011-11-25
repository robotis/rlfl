"""
"""
def print_map(MAP, ll, S, T):
    """
    """
    print('\n->')
    for row in range(len(MAP)):
        print('\t', end="")
        for col in range(len(MAP[row])):
            if (row, col) == S:
                print('S', end="")
            elif (row, col) == T:
                print('T', end="")
            elif (row, col) in ll:
                print('*', end="")
            else:
                print(MAP[row][col], end="")
        print()
    print('<-')
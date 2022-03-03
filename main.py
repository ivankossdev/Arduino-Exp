s1 = "()({[]}())" # true
s2 = "{}" # true
s3 = "}{" # false
s4 = "" # true
s5 = "(()"# false
s6 = "())"# false
s7 = "[(])" # false

def is_open(ch):
    if ch == "(" or ch == "{" or ch == "[":
        return True
    else:
        return False

def is_pair(open, close):
    if (open == "{" and close == "}") or (open == "(" and close == ")") or (open == "[" and close == "]"):
        return True
    else:
        return False

def stack_top(s):
    return ''.join(s[len(s)-1:])

def is_balanced(s):
    st = []
    for x in s:
        if is_open(x):
            st.append(x)
        else:
            if '' != st and is_pair(stack_top(st), x):
                st.pop()
            else:
                return False
    return True

print(is_balanced("()()()(){}"))

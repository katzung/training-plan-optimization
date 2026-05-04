# Вспомогательная функция считывания решения из .sol файла в модель
function parse_scip_solution(sol_path::String)                                     
	obj_val = nothing                         
	var_vals = Dict{String, Float64}()                                             
	for line in eachline(sol_path)    
		line = strip(line)        
		isempty(line) || startswith(line, "#") && continue                         
		if startswith(line, "objective value:")           
			obj_val = parse(Float64, split(line)[end])                             
		elseif !startswith(line, "solution status:")                               
			parts = split(line)                     
			if length(parts) >= 2                                                  
				var_vals[parts[1]] = parse(Float64, parts[2])
			end                                              
		end                                                                        
	end    
	return obj_val, var_vals                                                       
end

function parse_localmip_solution(sol_path::String)
    return parse_scip_solution(sol_path)
end

function read_input(file_path::String, validate::Bool)
    lines = readlines(file_path)

    lines = filter(line -> !isempty(line) && !startswith(strip(line), "#"), lines)
    lines = map(line -> split(line, "#")[1] |> strip, lines)

    idx = 1

    D = parse(Int, lines[idx]); idx += 1
    D_ = parse.(Int, split(lines[idx])); idx += 1
    D_star = parse.(Int, split(lines[idx])); idx += 1
    P = parse(Int, lines[idx]); idx += 1
        Player_names = split(lines[idx]); idx += 1
    S = parse(Int, lines[idx]); idx += 1
        Skill_names = split(lines[idx]); idx += 1
    T = parse(Int, lines[idx]); idx += 1
        Tactic_names = split(lines[idx]); idx += 1
    E = parse(Int, lines[idx]); idx += 1
        Exercise_names = split(lines[idx]); idx += 1

    a0 = parse.(Int, split(lines[idx])); idx += 1
    b0 = [parse.(Int, split(lines[idx + i - 1])) for i in 1:S]; idx += S
    c0 = [parse.(Int, split(lines[idx + i - 1])) for i in 1:T]; idx += T
    h = [parse.(Int, split(lines[idx + i - 1])) for i in 1:P]; idx += P
    w = parse.(Int, split(lines[idx])); idx += 1
    b = [parse.(Int, split(lines[idx + i - 1])) for i in 1:S]; idx += S
    c = [parse.(Int, split(lines[idx + i - 1])) for i in 1:T]; idx += T
    v = parse.(Int, split(lines[idx])); idx += 1
    V = parse.(Int, split(lines[idx])); idx += 1

    B_star = Array{Int, 3}(undef, S, P, length(D_star))
    for d in 1:length(D_star)
        for s in 1:S
            B_star[s, :, d] = parse.(Int, split(lines[idx])); idx += 1
        end
    end

    C_star = Array{Int, 3}(undef, T, P, length(D_star))
    for d in 1:length(D_star)
        for t in 1:T
            C_star[t, :, d] = parse.(Int, split(lines[idx])); idx += 1
        end
    end

    # input validation
    @assert maximum(D_) <= D
    @assert minimum(D_) >= 1
    @assert maximum(D_star) <= D
    @assert minimum(D_star) >= 1
    @assert length(Player_names) == P
    @assert length(Skill_names) == S
    @assert length(Tactic_names) == T
    @assert length(Exercise_names) == E
    @assert length(a0) == P
    @assert size(b0) == (S,) && size(b0[1]) == (P,)
    @assert size(c0) == (T,) && size(c0[1]) == (P,)
    @assert size(h) == (P,) && size(h[1]) == (D,)
    @assert length(w) == E
    @assert size(b) == (S,) && size(b[1]) == (E,)
    @assert size(c) == (T,) && size(c[1]) == (E,)
    @assert length(v) == E
    @assert length(V) == D
    @assert size(B_star) == (S, P, length(D_star))
    @assert size(C_star) == (T, P, length(D_star))

    return D, D_, D_star, P, Player_names, S, Skill_names, T, Tactic_names, E, Exercise_names, a0, b0, c0, h, w, b, c, v, V, B_star, C_star
end

function δ1(d_::Int, d::Int)
	return k1*ℯ^(-(d-d_)/τ1)
end

function δ2(d_::Int, d::Int)
	return k2*ℯ^(-(d-d_)/τ2)
end

function β(s::Int, p::Int, d::Int)
	return 1
end

function γ(t::Int, d::Int)
	return 1
end

function ρ(t::Int)
	return 2
end

function setup_model(input_file_path::String)
    D, D_, D_star, P, Player_names, S, Skill_names, T, Tactic_names, E, Exercise_names, a0, b0, c0, h, w, b, c, v, V, B_star, C_star = read_input(input_file_path, false)

    # Модель
    model = Model(SCIP.Optimizer)
    
    # Переменные
    @variable(model, x[1:E, 1:length(D_)], Bin) #>= 0, Int)
    @variable(model, y[1:S, 1:P, 1:length(D_star)], Bin)
    @variable(model, z[1:T, 1:P, 1:length(D_star)], Bin)
    @variable(model, Z[1:T, 1:length(D_star)], Bin)
    @variable(model, A_min[1:length(D_star)])

    # Вспомогательные функции и компоненты целевой функции
    function A(p::Int, d::Int)
        result = a0[p]
        result += sum(δ1(D_[d_], d)*h[p][d_]*(sum(w[e]*x[e, d_] for e in 1:E)) for d_ in 1:length(D_) if D_[d_] <= d)
        result -= sum(δ2(D_[d_], d)*h[p][d_]*(sum(w[e]*x[e, d_] for e in 1:E)) for d_ in 1:length(D_) if D_[d_] <= d)
        return result
    end

    function A_avg(d::Int)
        return sum(A(p, d) for p in 1:P)/P
    end

    function A()
        return sum(α*A_min[d_]+(1-α)*A_avg(D_star[d_]) for d_ in 1:length(D_star))
    end

    function B(s::Int, p::Int, d::Int)
        result = b0[s][p]
        result += sum(h[p][d_]*sum(b[s][e]*x[e, d_] for e in 1:E) for d_ in 1:length(D_) if D_[d_] <= d)
        return result
    end

    function B()
        return sum(sum(sum(β(s, p, d_)*y[s, p, d_] for s in 1:S) for p in 1:P) for d_ in 1:length(D_star))
    end

    function C(t::Int, p::Int, d::Int)
        result = c0[t][p]
        result += sum(h[p][d_]*sum(c[t][e]*x[e, d_] for e in 1:E) for d_ in 1:length(D_) if D_[d_] <= d)
    end

    function C()
        return sum(sum(γ(t, d_)*Z[t, d_] for t in 1:T) for d_ in 1:length(D_star))
    end

    # Ограничения
    @constraint(model, [d_=1:length(D_)], sum(v[e]*x[e, d_] for e in 1:E) <= V[d_])
    @constraint(model, [s=1:S, p=1:P, d_=1:length(D_star)], B(s, p, D_star[d_]) + B_star[s, p, d_]*(1-y[s, p, d_]) >= B_star[s, p, d_])
    @constraint(model, [t=1:T, p=1:P, d_=1:length(D_star)], C(t, p, D_star[d_]) + C_star[t, p, d_]*(1-z[t, p, d_]) >= C_star[t, p, d_])
    @constraint(model, [t=1:T, d_=1:length(D_star)], sum(z[t, p, d_] for p in 1:P) >= ρ(t)*Z[t, d_])
    @constraint(model, [d_=1:length(D_star), p=1:P], A_min[d_] <= A(p, D_star[d_]))

    return model, x, y, z, Z, A, A_avg, B, C
end

function compute_ideal_point(model)

    # Высчитывание идеальной точки: A
    @objective(model, Max, A())
    set_optimizer_attribute(model, "limits/gap", 0.01)
    set_optimizer_attribute(model, "limits/time", 1200.0) # 20 minutes
    optimize!(model)
    A_optimal = value(A())
    println("A solve complete")
    println(termination_status(model))
    if termination_status(model) == TIME_LIMIT
        gap = relative_gap(model)
        println("Relative gap: $(gap * 100)%")
    end

    # Высчитывание идеальной точки: B
    @objective(model, Max, B())
    problem_name = "ideal_B"
    mps_path = problem_name * ".mps"
    sol_path = problem_name * ".sol"
    write_to_file(model, mps_path)
    println("Model written to: $mps_path")
    # run(`scip -c "read $mps_path" -c "set parallel maxnthreads 14" -c "set presolving emphasis aggressive" -c "set separating emphasis aggressive" -c "set heuristics emphasis aggressive" -c "set limits gap 0.01" -c "set limits time 3600" -c "concurrentopt" -c "write solution $sol_path" -c "quit"`)
    run(`./Local-MIP --model_file $mps_path --sol_path $sol_path --time_limit 3600`)
    println("B solve complete")
    obj_val, var_vals = parse_localmip_solution(sol_path)
    B_optimal = obj_val

    # Высчитывание идеальной точки: C
    @objective(model, Max, C())
    set_optimizer_attribute(model, "limits/gap", 0.01)
    set_optimizer_attribute(model, "limits/time", 1200.0) # 20 minutes
    optimize!(model)
    C_optimal = value(C())
    println("C solve complete")
    println(termination_status(model))
    if termination_status(model) == TIME_LIMIT
        gap = relative_gap(model)
        println("Relative gap: $(gap * 100)%")
    end

    return [A_optimal, B_optimal, C_optimal]
end

# Generate weight combinations via spherical parameterization
# alphaaa = cos(φ₁), betaaa = sin(φ₁)cos(φ₂), gammaaa = sin(φ₁)sin(φ₂)
# φ₁, φ₂ ∈ {0, π/8, π/4, 3π/8, π/2}  →  25 combinations
function generate_weights_spherical()
    weights_for_moa = Tuple{Float64,Float64,Float64}[]
    for phi1 in range(0, pi/2, step=pi/8)
        for phi2 in range(0, pi/2, step=pi/8)
            push!(weights_for_moa, (cos(phi1), sin(phi1)*cos(phi2), sin(phi1)*sin(phi2)))
        end
    end
    println("Weight combinations: $(length(weights_for_moa))")
    for (i, (a, b, c)) in enumerate(weights_for_moa)
        println("  $i: A=$(round(a,digits=3))  B=$(round(b,digits=3))  C=$(round(c,digits=3))")
    end

    return weights_for_moa
end

